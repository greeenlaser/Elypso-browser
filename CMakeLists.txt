﻿cmake_minimum_required(VERSION 3.29.2)

# Ensure install path is set correctly
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out/install")
endif()

# Enable hot reload for MSVC
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
# Explicitly set the MSVC toolset for Visual Studio
if (MSVC)
    set(CMAKE_GENERATOR_TOOLSET "v143")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso Browser" VERSION ${PROJECT_VERSION})

# Set runtime library type for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "MSVC Runtime" FORCE)
endif()

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

# Physics files
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# External files
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}/_external_shared/GLFW")
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Glad")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}/_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}/_external_shared/ImGui")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Stb_image")
set(EXT_BROTLI_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Brotli")
set(EXT_LEXBOR_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Lexbor")
set(EXT_LIBCURL_DIR "${CMAKE_SOURCE_DIR}/_external_shared/LibCurl")
set(EXT_LIBDATACHANNEL_DIR "${CMAKE_SOURCE_DIR}/_external_shared/LibDataChannel")
set(EXT_LIBPSL_DIR "${CMAKE_SOURCE_DIR}/_external_shared/LibPSL")
set(EXT_LIBWEBSOCKETS_DIR "${CMAKE_SOURCE_DIR}/_external_shared/LibWebSockets")
set(EXT_NGHTTP2_DIR "${CMAKE_SOURCE_DIR}/_external_shared/NGHTTP2")
set(EXT_OPENSSL_DIR "${CMAKE_SOURCE_DIR}/_external_shared/OpenSSL")
set(EXT_SQLITE_DIR "${CMAKE_SOURCE_DIR}/_external_shared/SQLite")
set(EXT_ZSTD_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Zstd")
set(EXT_SCITER_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Sciter")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
set(GLFW_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_GLFW_DIR}/debug/glfw3dll.lib" , 
    "${EXT_GLFW_DIR}/release/glfw3dll.lib" > )
# Add Brotli
set(BROTLI_LIBRARY1_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_BROTLI_DIR}/debug/brotlicommon.lib" , 
    "${EXT_BROTLI_DIR}/release/brotlicommon.lib" > )
set(BROTLI_LIBRARY2_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_BROTLI_DIR}/debug/brotlidec.lib" , 
    "${EXT_BROTLI_DIR}/release/brotlidec.lib" > )
set(BROTLI_LIBRARY3_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_BROTLI_DIR}/debug/brotlienc.lib" , 
    "${EXT_BROTLI_DIR}/release/brotlienc.lib" > )
# Add Lexbor
set(LEXBOR_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_LEXBOR_DIR}/debug/lexbor.lib" , 
    "${EXT_LEXBOR_DIR}/release/lexbor.lib" > )
# Add LibCurl
set(LIBCURL_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_LIBCURL_DIR}/debug/libcurl-d_imp.lib" , 
    "${EXT_LIBCURL_DIR}/release/libcurl_imp.lib" > )
# Add LibDataChannel
set(LIBDATACHANNEL_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_LIBDATACHANNEL_DIR}/debug/datachannel.lib" , 
    "${EXT_LIBDATACHANNEL_DIR}/release/datachannel.lib" > )
# Add LibPSL
set(LIBPSL_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_LIBPSL_DIR}/debug/psl.lib" , 
    "${EXT_LIBPSL_DIR}/release/psl.lib" > )
# Add LibWebSockets
set(LIBWEBSOCKETS_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_LIBWEBSOCKETS_DIR}/debug/websockets.lib" , 
    "${EXT_LIBWEBSOCKETS_DIR}/release/websockets.lib" > )
# Add NGHTTP2
set(NGHTTP2_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_NGHTTP2_DIR}/debug/nghttp2.lib" , 
    "${EXT_NGHTTP2_DIR}/release/nghttp2.lib" > )
# Add OpenSSL
set(OPENSSL_LIBRARY1_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_OPENSSL_DIR}/debug/libcrypto.lib" , 
    "${EXT_OPENSSL_DIR}/release/libcrypto.lib" > )
set(OPENSSL_LIBRARY2_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_OPENSSL_DIR}/debug/libssl.lib" , 
    "${EXT_OPENSSL_DIR}/release/libssl.lib" > )
# Add SQLite
set(SQLITE_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_SQLITE_DIR}/debug/sqlite3.lib" , 
    "${EXT_SQLITE_DIR}/release/sqlite3.lib" > )
# Add Zstd
set(ZSTD_LIBRARY_PATH $<IF:$<CONFIG:Debug>, 
    "${EXT_ZSTD_DIR}/debug/zstd.lib" , 
    "${EXT_ZSTD_DIR}/release/zstd.lib" > )

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad)

# Add source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*/*.cpp
)

# Add external source files
list(APPEND SOURCE_FILES
	${EXT_GLAD_DIR}/glad.c
	${EXT_IMGUI_DIR}/imgui.cpp
	${EXT_IMGUI_DIR}/imgui_draw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_glfw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_opengl3.cpp
	${EXT_IMGUI_DIR}/imgui_tables.cpp
	${EXT_IMGUI_DIR}/imgui_widgets.cpp
	${EXT_STB_IMAGE_DIR}/stb_image.cpp
)

# Build as browser executable
add_executable(ElypsoBrowser ${SOURCE_FILES})

set_source_files_properties(${RESOURCE_FILE} PROPERTIES LANGUAGE RC)
target_sources(ElypsoBrowser PRIVATE ${RESOURCE_FILE})

# Forces the name to be Elypso browser
set_target_properties(ElypsoBrowser PROPERTIES OUTPUT_NAME "Elypso browser")

target_compile_features(ElypsoBrowser PRIVATE cxx_std_20)

# Apply all directories to target
target_include_directories(ElypsoBrowser PRIVATE
	# Internal code

    ${INCLUDE_DIR}
	${INCLUDE_DIR}/core
	${INCLUDE_DIR}/files
	${INCLUDE_DIR}/graphics
	${INCLUDE_DIR}/gui
	${INCLUDE_DIR}/utils
	${INCLUDE_DIR}/networking
	
	# External libraries
    ${EXT_GLFW_DIR}
	${EXT_GLAD_DIR}
	${EXT_GLM_DIR}
	${EXT_IMGUI_DIR}
	${EXT_STB_IMAGE_DIR}
	${EXT_BROTLI_DIR}
	${EXT_LEXBOR_DIR}
	${EXT_LIBCURL_DIR}
	${EXT_LIBDATACHANNEL_DIR}
	${EXT_LIBPSL_DIR}
	${EXT_LIBWEBSOCKETS_DIR}
	${EXT_NGHTTP2_DIR}
	${EXT_OPENSSL_DIR}
	${EXT_SQLITE_DIR}
	${EXT_ZSTD_DIR}
	${EXT_SCITER_DIR}
)

target_link_libraries(ElypsoBrowser PRIVATE 
	external_libs 
	${GLFW_LIBRARY_PATH}
	${BROTLI_LIBRARY1_PATH}
	${BROTLI_LIBRARY2_PATH}
	${BROTLI_LIBRARY3_PATH}
	${LEXBOR_LIBRARY_PATH}
	${LIBCURL_LIBRARY_PATH}
	${LIBDATACHANNEL_LIBRARY_PATH}
	${LIBPSL_LIBRARY_PATH}
	${LIBWEBSOCKETS_LIBRARY_PATH}
	${NGHTTP2_LIBRARY_PATH}
	${OPENSSL_LIBRARY1_PATH}
	${OPENSSL_LIBRARY2_PATH}
	${SQLITE_LIBRARY_PATH}
	${ZSTD_LIBRARY_PATH}
)

# Define GLFW_INCLUDE_NONE
target_compile_definitions(ElypsoBrowser PRIVATE GLFW_INCLUDE_NONE)

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS ElypsoBrowser DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "files" directory to the install directory after build
add_custom_command(TARGET ElypsoBrowser POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:ElypsoBrowser>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files $<TARGET_FILE_DIR:ElypsoBrowser>/files
)

# Copy all DLLs to the install directory after build
if(NOT CMAKE_CONFIGURATION_TYPES)  # This is for single-config generators (like Ninja, Makefiles)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/release")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/debug")
    endif()
else()  # Multi-config generators (like Visual Studio, Xcode)
    set(DLL_ORIGIN_DIR "$<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/files/external dlls/debug,${CMAKE_SOURCE_DIR}/files/external dlls/release>")
endif()

set(DLL_TARGET_DIR "$<TARGET_FILE_DIR:ElypsoBrowser>")

# Find all DLL files at configure time (works for single-config generators)
if(NOT CMAKE_CONFIGURATION_TYPES)
    file(GLOB DLL_FILES "${DLL_ORIGIN_DIR}/*.dll")

    foreach(DLL_FILE ${DLL_FILES})
        get_filename_component(DLL_NAME ${DLL_FILE} NAME)
        add_custom_command(TARGET ElypsoBrowser POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
                "${DLL_FILE}" 
                "${DLL_TARGET_DIR}/${DLL_NAME}"
        )
    endforeach()
else()
    # Multi-config generators: Copy DLLs at build time using generator expressions
    add_custom_command(TARGET ElypsoBrowser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DLL_ORIGIN_DIR}"
            "${DLL_TARGET_DIR}"
    )
endif()

set(COPY_TARGETS LICENSE.md EULA.md README.md SECURITY.md LIBRARIES.md)
foreach(FILE ${COPY_TARGETS})
  add_custom_command(TARGET ElypsoBrowser POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:ElypsoBrowser>/${FILE}
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_SOURCE_DIR}/${FILE}
	$<TARGET_FILE_DIR:ElypsoBrowser>/${FILE}
  )
endforeach()

# Include CPack to enable packaging
include(CPack)