
cmake_minimum_required(VERSION 3.12)

# define a macro that helps defining an option
macro(sfml_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# set a default build type if none was provided
# this has to be done before the project() instruction!
sfml_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")

# project name
project(SFML)

# include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# setup version numbers
set(VERSION_MAJOR 2)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# add the SFML header path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# add an option for choosing the build type (shared or static)
sfml_set_option(BUILD_SHARED_LIBS TRUE BOOL "TRUE to build SFML as shared libraries, FALSE to build it as static libraries")

# add an option for building the examples
sfml_set_option(SFML_BUILD_EXAMPLES FALSE BOOL "TRUE to build the SFML examples, FALSE to ignore them")

# add an option for building the API documentation
sfml_set_option(SFML_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")

# define SFML_STATIC if the build type is not set to 'shared'
if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DSFML_STATIC)
endif()

if(LINUX)
    if(BUILD_SHARED_LIBS)
        sfml_set_option(SFML_INSTALL_PKGCONFIG_FILES TRUE BOOL "TRUE to automatically install pkg-config files so other projects can find SFML")
        if(SFML_INSTALL_PKGCONFIG_FILES)
            foreach(sfml_module IN ITEMS all system window graphics audio network)
                CONFIGURE_FILE(
                    "tools/pkg-config/sfml-${sfml_module}.pc.in"
                    "tools/pkg-config/sfml-${sfml_module}.pc"
                    @ONLY)
	    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/tools/pkg-config/sfml-${sfml_module}.pc"
                    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig")
            endforeach()
        endif()
    else()
        if(SFML_INSTALL_PKGCONFIG_FILES)
            message(WARNING "No pkg-config files are provided for the static SFML libraries (SFML_INSTALL_PKGCONFIG_FILES will be ignored).")
        endif()
    endif()
endif()

# enable project folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# add the subdirectories
add_subdirectory(src/SFML)
if(SFML_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(SFML_BUILD_DOC)
    add_subdirectory(doc)
endif()

# setup the install rules
if(NOT SFML_BUILD_FRAMEWORKS)
    install(DIRECTORY include
            DESTINATION .
            COMPONENT devel
            PATTERN ".svn" EXCLUDE)
else()
    # find only "root" headers
    file(GLOB SFML_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "include/SFML/*")

    # in fact we have to fool cmake to copy all the headers in subdirectories
    # to do that we have to add the "root" headers to the PUBLIC_HEADER
    # then we can run a post script to copy the remaining headers

    # we need a dummy file in order to compile the framework
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp
                       COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

    set(SFML_SOURCES ${SFML_HEADERS})
    list(APPEND SFML_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

    # create SFML.framework
    add_library(SFML ${SFML_SOURCES})

    # edit target properties
    set_target_properties(SFML PROPERTIES 
                          FRAMEWORK TRUE
                          FRAMEWORK_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
                          MACOSX_FRAMEWORK_IDENTIFIER org.sfml-dev.SFML
                          MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
                          MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
                          PUBLIC_HEADER "${SFML_HEADERS}")

    # add the remaining headers
    add_custom_command(TARGET SFML 
                       POST_BUILD
                       COMMAND cp -r ${PROJECT_SOURCE_DIR}/include/SFML/* SFML.framework/Versions/${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}/Headers)

    # adapt install directory to allow distributing dylibs/frameworks in user’s frameworks/application bundle
    # NOTE : it's not required to link agains SFML.framework
    set_target_properties(SFML PROPERTIES 
                          BUILD_WITH_INSTALL_RPATH 1 
                          INSTALL_NAME_DIR "@executable_path/../Frameworks")

    # install rule
    install(TARGETS SFML
            FRAMEWORK DESTINATION ${CMAKE_INSTALL_FRAMEWORK_PREFIX}
            COMPONENT devel)
endif()

install(FILES cmake/Modules/FindSFML.cmake DESTINATION ${INSTALL_MISC_DIR}/cmake/Modules)
install(FILES license.txt DESTINATION ${INSTALL_MISC_DIR})
install(FILES readme.txt DESTINATION ${INSTALL_MISC_DIR})