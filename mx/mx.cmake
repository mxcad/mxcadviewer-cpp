
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

if(WINDOWS AND _MX_BUILD_USE_MXCADLIB_DLL)
	set(mxarx_LIB MxArx)
	add_library(${mxarx_LIB}
    	STATIC
    	IMPORTED)
	set_target_properties(${mxarx_LIB}
		PROPERTIES
		IMPORTED_LOCATION_DEBUG ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxarx_LIB}${ODS_LIB_FILE_EXTNAME}
		IMPORTED_LOCATION_RELEASE ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxarx_LIB}${ODS_LIB_FILE_EXTNAME}
	)

	set(mxdraw_LIB MxDraw)
	add_library(${mxdraw_LIB}
    	STATIC
    	IMPORTED)
	set_target_properties(${mxdraw_LIB}
		PROPERTIES
		IMPORTED_LOCATION_DEBUG ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxdraw_LIB}${ODS_LIB_FILE_EXTNAME}
		IMPORTED_LOCATION_RELEASE ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxdraw_LIB}${ODS_LIB_FILE_EXTNAME}
	)

	set(mxge_LIB MxGe)
	add_library(${mxge_LIB}
    	STATIC
    	IMPORTED)
	set_target_properties(${mxge_LIB}
		PROPERTIES
		IMPORTED_LOCATION_DEBUG ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxge_LIB}${ODS_LIB_FILE_EXTNAME}
		IMPORTED_LOCATION_RELEASE ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxge_LIB}${ODS_LIB_FILE_EXTNAME}
	)

	set(mxbase_LIB MxBase)
	add_library(${mxbase_LIB}
    	STATIC
    	IMPORTED)
	set_target_properties(${mxbase_LIB}
		PROPERTIES
		IMPORTED_LOCATION_DEBUG ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxbase_LIB}${ODS_LIB_FILE_EXTNAME}
		IMPORTED_LOCATION_RELEASE ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxbase_LIB}${ODS_LIB_FILE_EXTNAME}
	)

	set(mxkernel_LIB MxKernel)
	add_library(${mxkernel_LIB}
    	STATIC
    	IMPORTED)
	set_target_properties(${mxkernel_LIB}
		PROPERTIES
		IMPORTED_LOCATION_DEBUG ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxkernel_LIB}${ODS_LIB_FILE_EXTNAME}
		IMPORTED_LOCATION_RELEASE ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxkernel_LIB}${ODS_LIB_FILE_EXTNAME}
	)


	list(APPEND mxcadlib_LIB_LINKS ${mxdraw_LIB} ${mxarx_LIB} ${mxge_LIB} ${mxbase_LIB} ${mxkernel_LIB})
endif()


message(STATUS "MX_LINK_LIBRARIES_DEBUG:" ${MX_LINK_LIBRARIES_DEBUG}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME})
message(STATUS "MX_LINK_LIBRARIES_RELEASE:" ${MX_LINK_LIBRARIES_RELEASE}/${ODS_LIB_FILE_NAME_PRV}${mxcadlib_LIB}${ODS_LIB_FILE_EXTNAME})

if(LINUX)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
	    #message(STATUS "xxxxxxxxx:" ${MX_DIR}/oda/c/lib/lnxX64_8.3)
		link_directories(
			${MX_DIR}/oda/c/lib/lnxX64_8.3
			${MX_DIR}/src/3rdparty/glfw-3.3.5/code/linux_build/install/lib
			${MX_DIR}/src/3rdparty/iconv/code/linux_build/install/lib
	)	
	else()
		link_directories(
			${MX_DIR}/oda/c/lib/lnxX64_8.3
			${MX_DIR}/src/3rdparty/glfw-3.3.5/code/linux_build/install/lib
			${MX_DIR}/src/3rdparty/iconv/code/linux_build/install/lib
		)
	endif()
	
		
	set (TD_DB_LIB                libTD_Db.a)
	set (TD_DR_EXLIB              TD_DrawingsExamplesCommon)
	set (TD_EXLIB                 TD_ExamplesCommon)
	set (TD_MODELER_LIB           ModelerGeometry)
	set (TD_BREPRENDERER_LIB      TD_BrepRenderer)
	set (TD_BR_LIB                TD_Br)
	set (TD_ACISBLDR_LIB          TD_AcisBuilder)
	set (TD_DBROOT_LIB            TD_DbRoot)
	set (TD_GS_LIB                TD_Gs)
	set (TD_GI_LIB                TD_Gi)
	set (TD_BREPBUILDER_LIB       TD_BrepBuilder)
	set (TD_GE_LIB                TD_Ge)
	set (TD_SPATIALINDEX_LIB      TD_SpatialIndex)
	set (TD_ROOT_LIB              TD_Root)
	set (TD_RASTERPROC_LIB        RasterProcessor)
	set (TD_RASTER_LIB            RxRasterServices)
	set (TH_THIRDPARTYRASTER_LIB  FreeImage)
	set ( TD_jpeg_LIB     TD_jpeg)
	set ( TD_png_LIB     TD_png)
	set (TH_TIFF_LIB              TD_tiff)

	set (TH_ZLIB_LIB              TD_Zlib)
	set (TH_libXML                libXML)
	set (TD_RDIMBLK_LIB           RecomputeDimBlock)
	set (TH_FT_LIB                FreeType)
	set (TD_ALLOC_LIB             TD_Alloc)
	set (TH_CONDITIONAL_LIBCRYPTO         libcrypto_st)

	set(TD_TXV_GLES2_LIB WinGLES2)

	set(TR_VEC_LIB TrVec)
	set(TR_TXR_GL2_LIB TrGL2)
	set(TR_RENDER_LIB TrRenderBase)

	set(TR_BASE_LIB TrBase)

	set (TD_DBENT_LIB              TD_DbEntities)
	set (TD_DBCORE_LIB      TD_DbCore)
	set (TD_DBIO_LIB               TD_DbIO)
	set (TD_SM_LIB                 TD_Sm)
	set (TD_AVE_LIB                TD_Ave)
	set (TD_SCENEOE_LIB            SCENEOE)
	set (TD_ACCAMERA_LIB           ACCAMERA)
	set (TD_ISM_LIB                ISM)
	set (TD_WIPEOUT_LIB            WipeOut)
	set (TD_MPOLYGON_LIB           AcMPolygonObj15)
	set (TD_ATEXT_LIB              ATEXT)
	set (TD_RTEXT_LIB              RText)
	set (TH_UTF_LIB              UTF)

	set (TCOMPONENTS_BREPMODELER_LIB  OdBrepModeler)
	set (TD_BREPBUILDERFILLER_LIB     TD_BrepBuilderFiller)

	set(TD_PDFIUMMODULE_LIB PDFiumModule)
	set(TH_PDFIUM_LIB TD_Pdfium)
	set(TD_DWF_IMPORT_LIB TD_Dwf7Import)
	set(TH_DWF7_TOOLKIT_LIB DwfToolkit)
	set(TH_DWF7_W3DTK_LIB W3dTk)
	set(TH_DWF7_WHIPTK_LIB WhipTk)
	set(TH_DWF7_CORE_LIB DwfCore)
	set(TH_PDFIMPORT TD_PdfImport)
	set(TH_PDFTOOLKIT TD_PDFToolkit)
	set(TH_PDFEXPORT TD_PdfExport)
	set(TH_QPDF qpdf)
	set(TH_RxPdfToRasterServices RxPdfToRasterServices)
	set(TD_2DEXPORT TD_2dExport)
	set(TD_PRCE pcre)
	set(TD_TF TD_Tf)
	set(TD_STSF stsflib)
	set(TD_ODDRAWING ODDrawing)
	set(TD_SvgExport TD_SvgExport)

    list(APPEND mxcadlib_LIB_LINKS
		${TD_DR_EXLIB}
		${TD_EXLIB}
		${TD_MODELER_LIB}
		${TCOMPONENTS_BREPMODELER_LIB}
		${TD_BREPBUILDERFILLER_LIB}
		${TD_BREPRENDERER_LIB}
		${TD_BR_LIB}
		${TD_ACISBLDR_LIB}
		${TD_RASTERPROC_LIB}
		${TD_RASTER_LIB}
		${TD_DBENT_LIB}
		${TD_DBIO_LIB}
		${TD_DB_LIB}
		${TD_DBENT_LIB}
		${TD_DBCORE_LIB}
		${TD_DBIO_LIB}
		${TD_SM_LIB}
		${TD_AVE_LIB}
		${TD_SCENEOE_LIB}
		${TD_ACCAMERA_LIB}
		${TD_ISM_LIB}
		${TD_WIPEOUT_LIB}
		${TD_MPOLYGON_LIB}
		${TD_ATEXT_LIB}
		${TD_RTEXT_LIB}
		${TD_DB_LIB}
		${TD_DBENT_LIB}
		${TD_DBENT_LIB}
		${TD_DBIO_LIB}
		${TD_DB_LIB}
		${TD_DBENT_LIB}
		${TD_DBCORE_LIB}
		${TD_DBIO_LIB}
		${TD_SM_LIB}
		${TD_AVE_LIB}
		${TD_SCENEOE_LIB}
		${TD_ACCAMERA_LIB}
		${TD_ISM_LIB}
		${TD_WIPEOUT_LIB}
		${TD_MPOLYGON_LIB}
		${TD_ATEXT_LIB}
		${TD_RTEXT_LIB}
		${TD_DB_LIB}
		${TD_DBENT_LIB}
		${TD_DBROOT_LIB}
		${TD_GS_LIB}
		${TD_GI_LIB}
		${TD_BREPBUILDER_LIB}
		${TD_GE_LIB}
		${TD_SPATIALINDEX_LIB}
		${TD_ROOT_LIB}
		${TH_THIRDPARTYRASTER_LIB}
		${TH_TIFF_LIB}
		${TH_ZLIB_LIB}
		${TH_libXML}
		${TD_RDIMBLK_LIB}
		${ODA_ADD_SISL}
		${TD_ALLOC_LIB}
		${TH_CONDITIONAL_LIBCRYPTO}
		${TH_UTF_LIB}
		${TD_DWF_IMPORT_LIB}
		${TH_DWF7_TOOLKIT_LIB}
		${TH_DWF7_W3DTK_LIB}
		${TH_DWF7_WHIPTK_LIB}
		${TH_DWF7_CORE_LIB}
		${TH_PDFIUM_LIB}
		${TH_RxPdfToRasterServices}
		${TD_PDFIUMMODULE_LIB}
		${TH_PDFIMPORT}
		${TH_PDFEXPORT}
		${TH_PDFTOOLKIT}
		${TH_QPDF}
		${TD_SvgExport}
		${TD_ODDRAWING}
		${TD_2DEXPORT}
		${TD_PRCE}
		${TH_FT_LIB}
		${TD_TF}
		${TD_png_LIB}
		${TD_jpeg_LIB}
		${TD_STSF}
		-liconv
		glfw3
		${X11_LIBRARIES}
		pthread
		dl
		z
    )
endif()