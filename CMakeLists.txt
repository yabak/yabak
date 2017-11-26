cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(yabak
        LANGUAGES CXX
        VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)



#==============================================================================
# External projects supports
#==============================================================================

include(ExternalProject)

macro(CompileExternalProject Name StaticLinks #[[CMakeArgs]])

    ExternalProject_Add(
        ${Name}
        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/${Name}"
        TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/${Name}/tmp"
        STAMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/${Name}/stamp"
        URL "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/${Name}.tar.gz"
        URL_MD5 "ef68674fb47a8b8e741b34e429d86e9d"
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/${Name}"
        CONFIGURE_COMMAND "./configure"
                          "--prefix" "${CMAKE_CURRENT_BINARY_DIR}/${Name}/install"
                          "--enable-static"
                          "--enable-shared=no"
                          ${ARGN}
        BUILD_COMMAND "make"
        BUILD_IN_SOURCE 1
        INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${Name}/install"
        INSTALL_COMMAND "make" "install"
        TEST_BEFORE_INSTALL 0
        TEST_AFTER_INSTALL 0
    )

    add_dependencies(${CMAKE_PROJECT_NAME} ${Name})
    target_include_directories(${CMAKE_PROJECT_NAME}
                               PUBLIC
                               BEFORE
                               "${CMAKE_CURRENT_BINARY_DIR}/${Name}/install/include")
    foreach(libName ${StaticLinks})
        if (WIN32 AND MSVC)
            target_link_libraries(${CMAKE_PROJECT_NAME}
                                  "${CMAKE_CURRENT_BINARY_DIR}/${Name}/install/lib/${libName}.lib")
        else()
            target_link_libraries(${CMAKE_PROJECT_NAME}
                                  "${CMAKE_CURRENT_BINARY_DIR}/${Name}/install/lib/lib${libName}.a")
        endif()
    endforeach()

endmacro()


#
# Compile embedded liblzma
#

set(SOURCE_FILES src/main.cpp src/sha256.cpp src/sha256.h)

add_executable(yabak ${SOURCE_FILES})

CompileExternalProject(
    xz-5.2.3
    lzma
    --enable-threads=yes
    --disable-xz
    --disable-xzdec
    --disable-lzmadec
    --disable-lzmainfo
    --disable-lzma-links
    --disable-scripts
    --disable-doc
)
