cmake_minimum_required (VERSION 2.8)

include (CheckCSourceCompiles)
project (ResInsight)

set (VIZ_MODULES_FOLDER_NAME Fwk/VizFwk)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(BUILD_SHARED_LIBS OFF CACHE BOOL "ERT: Build shared libraries")

################################################################################
# Setup the main platform defines
################################################################################
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DCVF_LINUX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DCVF_OSX)
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS 		"-DCVF_LINUX -pipe -Wextra -Woverloaded-virtual -Wformat -std=c++0x")
    set(CMAKE_CXX_FLAGS_DEBUG 	"-ggdb -g3 -O0 -DDEBUG -D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNO_DEBUG")
endif() 


################################################################################
# OpenMP
################################################################################
option (RESINSIGHT_USE_OPENMP "Enable OpenMP parallellization in the code" ON)
if (RESINSIGHT_USE_OPENMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        message(STATUS "Enabling OpenMP support")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
        message(STATUS "Disabling OpenMP support")
    endif()
endif()


################################################################################
# Version number
################################################################################
include (ResInsightVersion.cmake)


################################################################################
# cotire 
# Fully automated CMake module for build speedup
# https://github.com/sakra/cotire
################################################################################

option(RESINSIGHT_ENABLE_COTIRE "Use Cotire to speed up build process" OFF)
if(RESINSIGHT_ENABLE_COTIRE)
    set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

    include(cotire)
endif()


################################################################################
# ERT
################################################################################

if (NOT MSVC)
  # Linux: Optional configuration of externally installed ERT, requires path to libraries and includes
  set(RESINSIGHT_ERT_EXTERNAL_LIB_ROOT "" CACHE PATH "Path to installed ERT libraries")
  set(RESINSIGHT_ERT_EXTERNAL_INCLUDE_ROOT "" CACHE PATH "Path to installed ERT includes")
endif()

if (RESINSIGHT_ERT_EXTERNAL_LIB_ROOT OR RESINSIGHT_ERT_EXTERNAL_INCLUDE_ROOT)
  if (NOT(RESINSIGHT_ERT_EXTERNAL_LIB_ROOT AND RESINSIGHT_ERT_EXTERNAL_INCLUDE_ROOT))
    message(FATAL_ERROR "Both RESINSIGHT_ERT_EXTERNAL_LIB_ROOT and RESINSIGHT_ERT_EXTERNAL_INCLUDE_ROOT must be defined")
  endif()

  list(APPEND ERT_INCLUDE_DIRS
    ${RESINSIGHT_ERT_EXTERNAL_INCLUDE_ROOT}
  )

  list(APPEND ERT_LIBRARIES
	${RESINSIGHT_ERT_EXTERNAL_LIB_ROOT}/libecl.so
	${RESINSIGHT_ERT_EXTERNAL_LIB_ROOT}/libeclxx.so
	${RESINSIGHT_ERT_EXTERNAL_LIB_ROOT}/libecl_well.so
	${RESINSIGHT_ERT_EXTERNAL_LIB_ROOT}/libert_geometry.so
	${RESINSIGHT_ERT_EXTERNAL_LIB_ROOT}/libert_util.so
  )
  
else()

    # Disable install of ERT libs and headers, as Ert code is compiled and linked directly
    SET(INSTALL_ERT OFF CACHE BOOL "ERT: Install library")
    SET(BUILD_PYTHON OFF CACHE BOOL "ERT: Run py_compile on the python wrappers")
    SET(ERT_USE_OPENMP ${OPENMP_FOUND} CACHE BOOL "ERT: Compile using OpenMP")
    SET(ERT_BUILD_CXX ON CACHE BOOL "ERT: Enable build of CXX wrappers" FORCE)

    # Remember original state
    set(ORIGINAL_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
    
    if (MSVC)
      # Force static linking on Windows
      set(BUILD_SHARED_LIBS OFF)
      set(ERT_HAVE_UNISTD OFF) #If anyone has mingw installed
    else ()
      set(RESINSIGHT_ERT_EXTERNAL_SOURCE_ROOT "" CACHE STRING "Path to ERT CMakeList.txt (source path)")
       
      # Force dynamic linking on other platforms
      # Copy of libraries into install folder of ResInsight is done a bit further down in this file
      set(BUILD_SHARED_LIBS ON)
    endif()

    if (RESINSIGHT_ERT_EXTERNAL_SOURCE_ROOT)
        set(ERT_INCLUDE_ROOT "${RESINSIGHT_ERT_EXTERNAL_SOURCE_ROOT}")
        add_subdirectory(${ERT_SOURCE_PATH} ${CMAKE_BINARY_DIR}/ThirdParty/Ert)
    else()
        set(ERT_INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/ThirdParty/Ert)
        add_subdirectory(ThirdParty/Ert)
    endif ()

    list(APPEND ERT_INCLUDE_DIRS
        ${ERT_INCLUDE_ROOT}/libecl/include/
        ${ERT_INCLUDE_ROOT}/libert_util/include/
        ${ERT_INCLUDE_ROOT}/libgeometry/include/
        ${ERT_INCLUDE_ROOT}/libecl_well/include/
        ${ERT_INCLUDE_ROOT}/libeclxx/include
        ${ERT_INCLUDE_ROOT}/libert_utilxx/include

        ${CMAKE_BINARY_DIR}/ThirdParty/Ert/libert_util/include
    )

    list(APPEND ERT_LIBRARIES
	        ecl 
	        eclxx
	        ecl_well 
	        ert_geometry 
	        ert_util  
    )

    if (MSVC)
      set_property(TARGET  
          ${ERT_LIBRARIES}
          ecl_lfs
	      PROPERTY FOLDER "Thirdparty/ERT"
      )
    endif()
    
    # Restore original state
    set(BUILD_SHARED_LIBS ${ORIGINAL_BUILD_SHARED_LIBS})
    
endif(RESINSIGHT_ERT_EXTERNAL_LIB_ROOT OR RESINSIGHT_ERT_EXTERNAL_INCLUDE_ROOT)


################################################################################ 
# Subset of Boost, used by NRLib and flow diagnostics application 
################################################################################ 
 
if(MSVC)
    # stop MSVC from trying to auto-link libs
    add_definitions("-DBOOST_ALL_NO_LIB") 
endif() 
 
add_subdirectory(ThirdParty/boost-Subset) 

list(APPEND THIRD_PARTY_LIBRARIES
    boost-Subset
)

################################################################################
# Opm
################################################################################

add_subdirectory(ThirdParty/custom-opm-flowdiagnostics)
add_subdirectory(ThirdParty/custom-opm-flowdiag-app)

list(APPEND OPM_LIBRARIES
    custom-opm-flowdiagnostics
    custom-opm-flowdiag-app
)

set_property(TARGET  
    ${OPM_LIBRARIES}
	PROPERTY FOLDER "Thirdparty/OPM"
)

################################################################################
# NRLib
################################################################################

add_subdirectory(ThirdParty/NRLib)

list(APPEND THIRD_PARTY_LIBRARIES
    NRLib
)


################################################################################
# Qt
################################################################################
set (QT_COMPONENTS_REQUIRED QtCore QtGui QtMain QtOpenGl QtNetwork QtScript QtScriptTools)
find_package (Qt4 COMPONENTS ${QT_COMPONENTS_REQUIRED})
if ( NOT QT4_FOUND )
    message(FATAL_ERROR "Package Qt4 is required, but not found. Please specify qmake for variable QT_QMAKE_EXECUTABLE")
endif ( NOT QT4_FOUND )

if (QT_VERSION_MINOR LESS 6)
    message(FATAL_ERROR "Qt 4.6 is required, please specify qmake for variable QT_QMAKE_EXECUTABLE")
endif()

include (${QT_USE_FILE})

# Open GL
find_package( OpenGL )

################################################################################
# Qwt
################################################################################

add_subdirectory(ThirdParty/Qwt/src)

list(APPEND THIRD_PARTY_LIBRARIES
    Qwt
)


################################################################################
# Nightcharts
################################################################################

add_subdirectory(ThirdParty/nightcharts)

list(APPEND THIRD_PARTY_LIBRARIES
    nightcharts
)


################################################################################
# Thirdparty libraries are put in ThirdParty solution folder
################################################################################
set_property(TARGET  
    ${THIRD_PARTY_LIBRARIES}
	PROPERTY FOLDER "Thirdparty"
)

################################################################################
# Vizualization Framework
################################################################################

# Allow use of non-threadsafe reference counter in cvf::Object on systems with no atomics support
if (CMAKE_COMPILER_IS_GNUCC)
 
    if (NOT DEFINED HAVE_GCC_SYNC_FUNCTIONS)
        check_c_source_compiles("int main(int argc, char **argv) {
            int a;
            __sync_add_and_fetch(&a, 1);
            __sync_fetch_and_add(&a, 1);
            __sync_sub_and_fetch(&a, 1);
            __sync_fetch_and_sub(&a, 1); }" HAVE_GCC_SYNC_FUNCTIONS)
    endif()
 
    if (HAVE_GCC_SYNC_FUNCTIONS)
        message("GCC synchronization functions detected")
    else()
        message("GCC synchronization functions NOT detected, fallback to non threadsafe reference counting")
        add_definitions(-DCVF_USE_NON_THREADSAFE_REFERENCE_COUNT)
    endif()
 
endif()

add_subdirectory(${VIZ_MODULES_FOLDER_NAME}/LibCore)
add_subdirectory(${VIZ_MODULES_FOLDER_NAME}/LibGeometry)
add_subdirectory(${VIZ_MODULES_FOLDER_NAME}/LibRender)
add_subdirectory(${VIZ_MODULES_FOLDER_NAME}/LibViewing)
add_subdirectory(${VIZ_MODULES_FOLDER_NAME}/LibGuiQt)

include_directories(
    ${LibCore_SOURCE_DIR}
    ${LibGeometry_SOURCE_DIR}
    ${LibRender_SOURCE_DIR}
    ${LibViewing_SOURCE_DIR}
    ${LibGuiQt_SOURCE_DIR}
)

list(APPEND VIZ_FWK_LIBRARIES
    LibGuiQt
    LibViewing
    LibRender
    LibGeometry
    LibCore
)

set_property(TARGET  
    ${VIZ_FWK_LIBRARIES}
	PROPERTY FOLDER "VizFwk"
)


################################################################################
# Application Framework
################################################################################

add_subdirectory(Fwk/AppFwk/cafAnimControl)
add_subdirectory(Fwk/AppFwk/cafViewer)

add_subdirectory(Fwk/AppFwk/cafProjectDataModel/cafPdmCore)
add_subdirectory(Fwk/AppFwk/cafProjectDataModel/cafPdmUiCore)
add_subdirectory(Fwk/AppFwk/cafProjectDataModel/cafPdmXml)
add_subdirectory(Fwk/AppFwk/cafProjectDataModel)

add_subdirectory(Fwk/AppFwk/cafCommand)
add_subdirectory(Fwk/AppFwk/cafUserInterface)
add_subdirectory(Fwk/AppFwk/cafPdmCvf)
add_subdirectory(Fwk/AppFwk/CommonCode)
add_subdirectory(Fwk/AppFwk/cafVizExtensions)

#add_subdirectory(Fwk/AppFwk/cafTests/cafTestCvfApplication)

add_subdirectory(Fwk/AppFwk/cafTensor)

list(APPEND APP_FWK_LIBRARIES
    cafPdmCore
    cafPdmUiCore
    cafPdmXml
    cafProjectDataModel
    
    cafUserInterface
    cafViewer
    cafAnimControl
    cafCommand
    cafPdmCvf

	cafTensor
    CommonCode
    cafVizExtensions
)

set_property(TARGET  
    ${APP_FWK_LIBRARIES}
	PROPERTY FOLDER "AppFwk"
)


################################################################################
# Installation settings
################################################################################

set (RESINSIGHT_INSTALL_FOLDER ".")

# override system install prefix if private installation chosen
option (RESINSIGHT_PRIVATE_INSTALL "Install as an independent bundle including the neccesary Qt libraries" ON)
if (RESINSIGHT_PRIVATE_INSTALL)
    set (CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Install/)
    
    ################################################################################
    # ERT shared library files
    # Install procedure will copy so-files from ERT into same install folder as ResInsight
    ################################################################################
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(ERT_SHARED_LIB_FILES
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libecl.so
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libecl.so.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libecl.so.2.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libeclxx.so
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libeclxx.so.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libeclxx.so.2.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libecl_well.so
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libecl_well.so.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libecl_well.so.2.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libert_geometry.so
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libert_geometry.so.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libert_geometry.so.2.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libert_util.so
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libert_util.so.2
            ${CMAKE_BINARY_DIR}/ThirdParty/Ert/${CMAKE_INSTALL_LIBDIR}/libert_util.so.2.2
        )
      
      install(FILES ${ERT_SHARED_LIB_FILES} DESTINATION ${RESINSIGHT_INSTALL_FOLDER} )
    endif()
    
endif (RESINSIGHT_PRIVATE_INSTALL)


################################################################################
# Application 
################################################################################
add_subdirectory(ApplicationCode)
add_subdirectory(OctavePlugin)

################################################################################
# Code documentation using Doxygen
################################################################################
option(RESINSIGHT_BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(RESINSIGHT_BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR 
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (Docs ALL 
  					COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
  					SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
endif(RESINSIGHT_BUILD_DOCUMENTATION)

################################################################################
# Installation packaging
################################################################################

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CPACK_GENERATOR TGZ)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CPACK_GENERATOR ZIP)
endif()

if(RESINSIGHT_OCTAVE_PLUGIN_MKOCTFILE) 
    get_filename_component(RESINSIGHT_OCTAVE_DIRECTORY ${RESINSIGHT_OCTAVE_PLUGIN_MKOCTFILE} DIRECTORY)
    
    execute_process (COMMAND octave-config -v
        WORKING_DIRECTORY ${RESINSIGHT_OCTAVE_DIRECTORY}
                    OUTPUT_VARIABLE OCTAVE_VERSION_STRING
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif ()

# Handling of system name on Windows
if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
  if(CMAKE_CL_64)
    set(CPACK_SYSTEM_NAME win64)
  else()
    set(CPACK_SYSTEM_NAME win32)
  endif()
endif()

# Append el5 when compiled on RHEL5 and el6 if compiled on RHEL6
string(REGEX MATCH "el[5,6]?" RESINSIGHT_RHEL_SYSTEM_NAME ${CMAKE_SYSTEM})


set(RESINSIGHT_PACKAGE_NAME "ResInsight")

set (RESINSIGHT_PACKAGE_NAME "${RESINSIGHT_PACKAGE_NAME}-${STRPRODUCTVER}")

if(NOT ${RESINSIGHT_ODB_API_DIR} EQUAL "")
    set (RESINSIGHT_PACKAGE_NAME "${RESINSIGHT_PACKAGE_NAME}_odb")
endif()

if(NOT ${OCTAVE_VERSION_STRING} EQUAL "")
    set (RESINSIGHT_PACKAGE_NAME "${RESINSIGHT_PACKAGE_NAME}_oct-${OCTAVE_VERSION_STRING}")
endif()


# Append el5 when compiled on RHEL5 and el6 if compiled on RHEL6
if (NOT "${RESINSIGHT_RHEL_SYSTEM_NAME}" STREQUAL "")
    set (RESINSIGHT_PACKAGE_NAME "${RESINSIGHT_PACKAGE_NAME}_${RESINSIGHT_RHEL_SYSTEM_NAME}")
else()
    set (RESINSIGHT_PACKAGE_NAME "${RESINSIGHT_PACKAGE_NAME}_${CPACK_SYSTEM_NAME}")
endif()

#message("RESINSIGHT_PACKAGE_NAME : " ${RESINSIGHT_PACKAGE_NAME})
set(CPACK_PACKAGE_FILE_NAME ${RESINSIGHT_PACKAGE_NAME})


include (CPack)
