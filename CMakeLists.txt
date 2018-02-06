
# basis
cmake_minimum_required(VERSION 3.1)

set(project_name "obsOpencastIngestPlugin")

project(${project_name})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})

message("================================================")
message("Project: ${project_name}")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")


# obs lib
message("================================================")
find_package(LibObs REQUIRED)
message("Found: LibObs")
message("Include dirs: ${LIBOBS_INCLUDE_DIRS}")
message("Libraries: ${LIBOBS_LIBRARIES}")


# libOpencastIngestRestHelper lib
message("================================================")
set(LOIRH_INCLUDE_DIRS "libs/libOpencastIngest/include")
set(LOIRH_LIBRARIES_NAME "OpencastIngest")
set(LOIRH_LIBRARIES_PATH "libs/libOpencastIngest/lib")
find_library(LOIRH_LIBRARIES ${LOIRH_LIBRARIES_NAME} ${LOIRH_LIBRARIES_PATH})
message("Using: libOpencastIngestRestHelper")
message("Include dirs: ${LOIRH_INCLUDE_DIRS}")
message("Libraries: ${LOIRH_LIBRARIES}")


# curl lib (dependency of libOpencastIngestRestHelper(static version))
message("================================================")
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_package(CURL REQUIRED PATHS "/c/Program Files/")
	else()
		find_package(CURL REQUIRED PATHS "/c/Program Files (x86)/")
	endif()
else()
	find_package(CURL REQUIRED)
endif()
message("Found Curl version (dependency of libOpencastIngestRestHelper(static version)): ${CURL_VERSION_STRING}")
if(WIN32)
	message("Libraries (before adjustment): ${CURL_LIBRARIES}")
	set(CURL_LIBRARIES CURL::libcurl)
endif()
message("Libraries: ${CURL_LIBRARIES}")
if(WIN32)
	message("Other Settings (like includes) are set by adding the lib")
	set(CURL_INCLUDE_DIRS "")
else()
	message("Include dirs: ${CURL_INCLUDE_DIRS}")
endif()


# tinyxml2 lib (dependency of libOpencastIngestRestHelper(static version))
message("================================================")
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_package(TinyXML2 REQUIRED PATHS "/c/Program Files/")
	else()
		find_package(TinyXML2 REQUIRED PATHS "/c/Program Files (x86)/")
	endif()
else()
	find_package(TinyXML2 REQUIRED)
endif()
message("Found tinyxml2")
if(WIN32)
	message("Libraries (before adjustment): ${TINYXML2_LIBRARIES}")
	set(TINYXML2_LIBRARIES tinyxml2)
endif()
message("Libraries: ${TINYXML2_LIBRARIES}")
if(WIN32)
	message("Other Settings (like includes) are set by adding the lib")
else()
	message("Include dirs: ${TINYXML2_INCLUDE_DIR}")
endif()


# libconfig lib
message("================================================")
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_path(CONFIG++_INCLUDE_DIRS libconfig.h++ "C:/Program Files/libconfig/include/")
		find_library(CONFIG++_LIBRARY NAMES config++ config++.lib libconfig++ libconfig++.lib PATHS "C:/Program Files/libconfig/lib/")
	else()
		find_path(CONFIG++_INCLUDE_DIRS libconfig.h++ "C:/Program Files (x86)/libconfig/include/")
		find_library(CONFIG++_LIBRARY NAMES config++ config++.lib libconfig++ libconfig++.lib PATHS "C:/Program Files (x86)/libconfig/lib/")
	endif()
else()
	find_path(CONFIG++_INCLUDE_DIRS libconfig.h++ /usr/local/include)
	find_library(CONFIG++_LIBRARY config++ /usr/local/lib)
endif()
message("Using: Config++ (libconfig)")
message("Include dirs: ${CONFIG++_INCLUDE_DIRS}")
message("Libraries: ${CONFIG++_LIBRARY}")


# obs-frontend-api lib
message("================================================")
set(OBSFA_LIBRARIES_NAME "obs-frontend-api")
if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_library(OBSFA_LIBRARIES ${OBSFA_LIBRARIES_NAME} PATHS "../obs-studio/build/UI/obs-frontend-api/Release/")
	else()
		find_library(OBSFA_LIBRARIES ${OBSFA_LIBRARIES_NAME} PATHS "../obs-studio/build32/UI/obs-frontend-api/Release/")
	endif()
else()
	find_library(OBSFA_LIBRARIES ${OBSFA_LIBRARIES_NAME})
endif()
message("Using: obs-frontend-api")
message("Include dirs: not used (forward declaration see obsOpencastIngestPlugin.cpp)")
if (WIN32)
	message("Libraries: ${OBSFA_LIBRARIES}")
else()
	message("Libraries: ${OBSFA_LIBRARIES}(Currently not linked)")
endif()


# Qt5 lib
message("================================================")
if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_package(Qt5 REQUIRED Widgets PATHS "C:/Qt/5.9.2/msvc2017_64/lib/cmake/Qt5" NO_DEFAULT_PATH)
	else()
		find_package(Qt5 REQUIRED Widgets PATHS "C:/Qt/5.9.2/msvc2015/lib/cmake/Qt5" NO_DEFAULT_PATH)
	endif()
else()
	find_package(Qt5 REQUIRED Widgets)
endif()

message("Found: Qt5")

message("Using: Widgets")
set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5Widgets_LIBRARIES})

message("Libraries: ${Qt5_LIBRARIES}")
message("Qt5 other Settings (like includes) are set by adding the library")


# main qt

message("================================================")
set(main_qt_ui "src/ui/infoBox.ui" "src/ui/dialogopencast.ui" "src/ui/opencastSettings.ui")
qt5_wrap_ui(main_ui_HEADERS ${main_qt_ui})
message("Qt generating ui: ${main_qt_ui}")

foreach(loop_var IN ITEMS ${main_ui_HEADERS})
	get_filename_component(main_ui_HEADERS_PATH ${loop_var} DIRECTORY)
	set(main_ui_HEADERS_PATHS ${main_ui_HEADERS_PATHS} ${main_ui_HEADERS_PATH})
endforeach()

set(main_qt_moc "src/moc/guiLauncher.h" "src/moc/infoBox.h" "src/moc/dialogopencast.h" "src/moc/opencastSettings.h")
qt5_wrap_cpp(main_moc_SOURCE ${main_qt_moc})
message("Qt moc: ${main_qt_moc}")

set(main_qt_source "src/moc/guiLauncher.cpp" "src/moc/infoBox.cpp" "src/moc/dialogopencast.cpp" "src/moc/opencastSettings.cpp" ${main_qt_moc} ${main_moc_SOURCE} ${main_ui_HEADERS})
set(main_qt_include ${main_ui_HEADERS_PATHS})
message("Qt Source: ${main_qt_source}")
message("Qt Includes: ${main_qt_include}")


# git version

message("================================================")
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
	find_package(Git)
	if(GIT_FOUND)
		execute_process(COMMAND ${GIT_EXECUTABLE} describe
		    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		    OUTPUT_VARIABLE build_version
		    ERROR_QUIET
		    OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	else()
		set(build_version "BUILD_VERSION_NOT_FOUND")
	endif()
else()
	set(build_version "BUILD_VERSION_NOT_FOUND")
endif()
# currently unused
message("Build Version: ${build_version}" )


# main

message("================================================")

set(main_gnucxx_warn_err_compile_flags "-Wall -Wextra -pedantic-errors")

set(main_source "src/obsOpencastIngestPlugin.cpp" "src/obsOpencastIngestPlugin.hpp" "src/utilsAndConfig.cpp" "src/utilsAndConfig.hpp" ${main_qt_source})
set(main_include_dirs ${LIBOBS_INCLUDE_DIRS} ${LOIRH_INCLUDE_DIRS} ${CONFIG++_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${TINYXML2_INCLUDE_DIR} ${main_qt_include})
set(main_libraries ${LIBOBS_LIBRARIES} ${LOIRH_LIBRARIES} ${CONFIG++_LIBRARY} ${CURL_LIBRARIES} ${TINYXML2_LIBRARIES} ${Qt5_LIBRARIES})
if (WIN32)
	set(main_libraries ${main_libraries} ${OBSFA_LIBRARIES})
endif()

message("Name (lib using C++11): ${project_name}")
message("Used source: ${main_source}")
message("Include dirs: ${main_include_dirs}")
message("Libraries: ${main_libraries}")

add_library(${project_name} SHARED ${main_source})

if(CMAKE_COMPILER_IS_GNUCXX)
	set_target_properties(${project_name} PROPERTIES COMPILE_FLAGS ${main_gnucxx_warn_err_compile_flags})
endif()
set_target_properties(${project_name} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_compile_definitions(${project_name} PRIVATE -DOBSOPENCASTINGESTPLUGIN_BUILD_VERSION=\"${build_version}\")
target_include_directories(${project_name} PRIVATE ${main_include_dirs})
target_link_libraries(${project_name} PRIVATE ${main_libraries})
