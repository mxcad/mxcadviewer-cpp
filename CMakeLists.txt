cmake_minimum_required(VERSION 3.10)

project(MxCADViewer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    add_definitions(-D_USE_MATH_DEFINES) # Windows for math.h
endif()

option(MX_BUILD_3D "Build 3D" ON)
option(MX_BUILD_LOGIN "Build login system" OFF)

set(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/3rdparty" CACHE PATH "3rdparty directory")
set(MX_DIR "${CMAKE_SOURCE_DIR}/mx/code" CACHE PATH "mx directory")


# 3RDPARTY_DIR
if(NOT 3RDPARTY_DIR)
    message(FATAL_ERROR "3RDPARTY_DIR not set!")
endif()

if(NOT EXISTS "${3RDPARTY_DIR}")
    message(FATAL_ERROR "3RDPARTY_DIR not exists: ${3RDPARTY_DIR}")
endif()

if(NOT EXISTS "${3RDPARTY_DIR}/opencascade-7.7.0")
    message(FATAL_ERROR "3RDPARTY_DIR not correct!")
endif()

# MX_DIR
if(NOT MX_DIR)
    message(FATAL_ERROR "MX_DIR not set!")
endif()

if(NOT EXISTS "${MX_DIR}")
    message(FATAL_ERROR "MX_DIR not exists: ${MX_DIR}")
endif()

if(NOT EXISTS "${MX_DIR}/build_mxcadlib")
    message(FATAL_ERROR "MX_DIR not correct!")
endif()

# Add #define MX_BUILD_3D
if(MX_BUILD_3D)
    add_definitions(-DMX_BUILD_3D)
endif()

#Add #define MX_BUILD_LOGIN
if(MX_BUILD_LOGIN)
	add_definitions(-DMX_BUILD_LOGIN)
endif()


if(MX_BUILD_3D)
    # OpenCASCADE_DIR
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(OpenCASCADE_INC_DIR "${3RDPARTY_DIR}/opencascade-7.7.0/inc")
		set(OpenCASCADE_BINARY_DIR "${3RDPARTY_DIR}/opencascade-7.7.0/win64/vc14/bin")
		set(OpenCASCADE_LIBRARY_DIR "${3RDPARTY_DIR}/opencascade-7.7.0/win64/vc14/lib")
		set(OpenCASCADE_LIBRARY_DIR_DEBUG ${OpenCASCADE_LIBRARY_DIR}d)
		set(OpenCASCADE_LIBRARIES TKernel;TKMath;TKG2d;TKG3d;TKGeomBase;TKBRep;TKGeomAlgo;TKTopAlgo;TKPrim;TKBO;TKShHealing;TKBool;TKHLR;TKFillet;TKOffset;TKFeat;TKMesh;TKXMesh;TKService;TKV3d;TKOpenGl;TKMeshVS;TKCDF;TKLCAF;TKCAF;TKBinL;TKXmlL;TKBin;TKXml;TKStdL;TKStd;TKTObj;TKBinTObj;TKXmlTObj;TKVCAF;TKXDE;TKXSBase;TKSTEPBase;TKSTEPAttr;TKSTEP209;TKSTEP;TKIGES;TKXCAF;TKXDEIGES;TKXDESTEP;TKSTL;TKVRML;TKRWMesh;TKXmlXCAF;TKBinXCAF;TKXDECascade;TKExpress)
	    add_definitions(-D_USE_MATH_DEFINES) # Windows for math.h
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(OpenCASCADE_DIR "${3RDPARTY_DIR}/opencascade-7.7.0/lib/cmake/opencascade")
    endif()
endif()

# freetype
set(FT_LIB_DIR "${3RDPARTY_DIR}/freetype-2.5.5/lib")

# xlsxwriter
set(XLSXWRITER_INC_DIR "${3RDPARTY_DIR}/libxlsxwriter/include")
set(XLSXWRITER_LIB_DIR "${3RDPARTY_DIR}/libxlsxwriter/lib")
message(STATUS "XLSXWRITER_INC_DIR:" ${XLSXWRITER_INC_DIR})
message(STATUS "XLSXWRITER_LIB_DIR:" ${XLSXWRITER_LIB_DIR})
# WIN ZLIB
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(ZLIB_LIB_DIR "${3RDPARTY_DIR}/zlib-1.3.1/lib")
    message(STATUS "ZLIB_LIB_DIR:" ${ZLIB_LIB_DIR})
endif()

# find Qt and other libs
find_package(Qt5 COMPONENTS Core Gui Network Svg Widgets Concurrent PrintSupport LinguistTools REQUIRED)
find_package(OpenGL REQUIRED)



if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(Fontconfig REQUIRED)
    find_package(PNG REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(OpenSSL REQUIRED)
endif()


set(COMMON_DIR ${CMAKE_SOURCE_DIR}/src)
# COMMON Sources
file(GLOB COMMON_SOURCES 
    "${COMMON_DIR}/*.cpp" 
    "${COMMON_DIR}/*.h"
    "${COMMON_DIR}/*.qrc"
    "${COMMON_DIR}/*.rc"
)


if(MX_BUILD_3D)
	set(3D_DIR ${COMMON_DIR}/mxcad3d)
    # MXCAD3D Sources
    file(GLOB MXCAD3D_SOURCES 
        "${3D_DIR}/*.cpp"
        "${3D_DIR}/*.h"
    )
endif()

if(MX_BUILD_LOGIN)
	set(LOGIN_DIR ${COMMON_DIR}/mxlogin)
	#MXLOGIN Sources
	file(GLOB MXLOGIN_SOURCES
		"${LOGIN_DIR}/*.cpp"
		"${LOGIN_DIR}/*.h"
	)
endif()

# Add 
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
    add_definitions(-DMXLINUX)
    add_definitions(-D_MX_OPENGL_WINDOWS_NORMAL)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS TRUE)
    add_definitions(-D_MX_WINDOWS)
endif()

message(STATUS "_MX_BUILD_USE_MXCADLIB:" ${_MX_BUILD_USE_MXCADLIB})
message(STATUS "_MX_BUILD_USE_MXCADLIB_DLL:" ${_MX_BUILD_USE_MXCADLIB_DLL})

add_definitions(-D_MX_BUILD_USE_MXCADLIB)
include(${CMAKE_SOURCE_DIR}/mx/mx.cmake)
set(MXCAD_LINK_FILES ${mxcadlib_LIB_LINKS})

set(2D_DIR ${COMMON_DIR}/mxcad2d)

#2D Sources
file(GLOB MXCAD2D_SOURCES 
	"${2D_DIR}/*.cpp"
	"${2D_DIR}/*.h"
)


# Create executable
add_executable(${PROJECT_NAME} ${COMMON_SOURCES} ${LOGIN_SOURCES} ${MXCAD3D_SOURCES} ${MXLOGIN_SOURCES} ${MXCAD2D_SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCASCADE_INC_DIR} ${3RDPARTY_DIR} ${XLSXWRITER_INC_DIR} ${MXCAD_INC_DIR} ${COMMON_DIR} ${3D_DIR} ${LOGIN_DIR} ${2D_DIR})


#include(mxcadlib/CMakeLists222.txt)
# not console
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


# Set link directories
target_link_directories(${PROJECT_NAME} PRIVATE "$<IF:$<CONFIG:Debug>,${OpenCASCADE_LIBRARY_DIR_DEBUG},${OpenCASCADE_LIBRARY_DIR}>" ${FT_LIB_DIR} ${XLSXWRITER_LIB_DIR} ${ZLIB_LIB_DIR})


# link libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCASCADE_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${Freetype_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${PNG_LIBRARIES}
        ${Fontconfig_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${X11_LIBRARIES}
        -ldl
        ${MXCAD_LINK_FILES}
        OpenSSL::SSL
        OpenSSL::Crypto
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::Svg
        Qt5::Widgets
        Qt5::Concurrent
        Qt5::PrintSupport
        xlsxwriter
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCASCADE_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${MXCAD_LINK_FILES}
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::Svg
        Qt5::Widgets
        Qt5::Concurrent
        Qt5::PrintSupport
        xlsxwriter
        zlibstatic
		freetype
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>/licenses"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
	"${CMAKE_SOURCE_DIR}/src/resources/occt_qt_licenses"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>/licenses"
)

# copy mxconvert to target directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
	"${3RDPARTY_DIR}/mxconvert"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

if(MX_BUILD_LOGIN)
	# copy ssl dll to target directory
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
		"${3RDPARTY_DIR}/ssl"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND MSVC)

	set(MX_DEBUG_DIR "${MX_DIR}/build_mxcadlib/bin/Debug")
	set(MX_RELEASE_DIR "${MX_DIR}/build_mxcadlib/bin/Release")
	
	set(MX_DEBUG_mxcad_ini_json "${MX_DEBUG_DIR}/mxcad_ini.json")
	set(MX_DEBUG_mxcadlib_dll "${MX_DEBUG_DIR}/mxcadlib.dll")
	set(MX_DEBUG_mxiconv_data "${MX_DEBUG_DIR}/mxiconv.data")
	
	set(MX_RELEASE_mxcad_ini_json "${MX_RELEASE_DIR}/mxcad_ini.json")
	set(MX_RELEASE_mxcadlib_dll "${MX_RELEASE_DIR}/mxcadlib.dll")
	set(MX_RELEASE_mxiconv_data "${MX_RELEASE_DIR}/mxiconv.data")
	
	
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<IF:$<CONFIG:Debug>,${MX_DEBUG_mxcad_ini_json},${MX_RELEASE_mxcad_ini_json}>"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<IF:$<CONFIG:Debug>,${MX_DEBUG_mxcadlib_dll},${MX_RELEASE_mxcadlib_dll}>"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<IF:$<CONFIG:Debug>,${MX_DEBUG_mxiconv_data},${MX_RELEASE_mxiconv_data}>"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	)
	
	get_target_property (QtCore_location Qt5::Core LOCATION)
	get_filename_component (QT_BINARY_DIR ${QtCore_location} DIRECTORY)
	set (QT_PLUGINS_DIR)
	if (EXISTS "${QT_BINARY_DIR}/../plugins")
		set (QT_PLUGINS_DIR "${QT_BINARY_DIR}/../plugins")
	endif()
	
	set(OpenCASCADE_BINARY_DIR_DEBUG "${OpenCASCADE_BINARY_DIR}d")
	
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
		"$<IF:$<CONFIG:Debug>,${OpenCASCADE_BINARY_DIR_DEBUG},${OpenCASCADE_BINARY_DIR}>"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	)
	
	set_target_properties (${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "\
PATH=%PATH%;${QT_BINARY_DIR}\n\
QT_PLUGIN_PATH=${QT_PLUGINS_DIR}"
  )
  
	
endif()