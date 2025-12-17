
set(MXCAD_INC_DIR ${MX_DIR}/src/mxcadlib/MxCADLib/inc)


if(LINUX)
    set(MX_LINK_LIBRARIES_DEBUG ${MX_DIR}/linux_build_mxcadlib_dbg_opengl_window/lib)
    set(MX_LINK_LIBRARIES_RELEASE ${MX_DIR}/linux_build_mxcadlib_opengl_window/lib)

    set(ODS_LIB_FILE_EXTNAME ".a")
    set(ODS_LIB_FILE_NAME_PRV "lib")

elseif(WINDOWS)
    if(_MX_BUILD_USE_MXCADLIB_DLL)
        set(MX_LINK_LIBRARIES_DEBUG ${MX_DIR}/build_mxcadlib_dll/lib/Debug)
        set(MX_LINK_LIBRARIES_RELEASE ${MX_DIR}/build_mxcadlib_dll/lib/Release)
    else()
        set(MX_LINK_LIBRARIES_DEBUG ${MX_DIR}/build_mxcadlib/lib/Debug)
        set(MX_LINK_LIBRARIES_RELEASE ${MX_DIR}/build_mxcadlib/lib/Release)
    endif()
    set(ODS_LIB_FILE_EXTNAME ".lib")
    set(ODS_LIB_FILE_NAME_PRV "")
endif()

set(mxcadlib_LIB mxcadlib)
set(mxcadlib_LIB_LINKS mxcadlib)

add_library(${mxcadlib_LIB}
    STATIC
    IMPORTED)

if(LINUX)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		message(STATUS "MxCADLibDebug")
		set_target_properties(${mxcadlib_LIB}
			PROPERTIES
			IMPORTED_LOCATION ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME})
	else()
		message(STATUS "MxCADLibRelease")
		set_target_properties(${mxcadlib_LIB}
			PROPERTIES
			IMPORTED_LOCATION ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME}
		)
	endif()

else()
	set_target_properties(${mxcadlib_LIB}
		PROPERTIES
		IMPORTED_LOCATION_DEBUG ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME}
		IMPORTED_LOCATION_RELEASE ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME}
	)
endif()


message(STATUS "MX_LINK_LIBRARIES_DEBUG:" ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME})
message(STATUS "MX_LINK_LIBRARIES_RELEASE:" ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME})

if(LINUX)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		link_directories(
			${MX_DIR}/src/3rdparty/glfw-3.3.5/code/linux_build/install/lib
	)	
	else()
		link_directories(
			${MX_DIR}/src/3rdparty/glfw-3.3.5/code/linux_build/install/lib
		)
	endif()
	
		
endif()