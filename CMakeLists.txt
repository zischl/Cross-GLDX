cmake_minimum_required(VERSION 3.15)
project(CrossGLDX VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Architecture Detection
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()

# Source Definitions
# Better to list headers here so they show up in IDEs like Visual Studio
set(COMMON_SOURCES
    src/Logger.cpp
    src/Logger.h
)

set(WIN32_SOURCES
    src/D3D11Renderer.cpp
    src/D3D11Renderer.h
)

# Target Setup
if(WIN32)
    set(SOURCES ${COMMON_SOURCES} ${WIN32_SOURCES})
else()
    set(SOURCES ${COMMON_SOURCES})
endif()

add_library(CrossGLDX STATIC ${SOURCES})
target_include_directories(CrossGLDX PUBLIC src)

# Dependencies

file(GLOB EXTERNAL_DLIBS "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/Debug/${ARCH}/*.lib")

file(GLOB EXTERNAL_RLIBS "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/Release/${ARCH}/*.lib")

file(GLOB EXTERNAL_ULIBS "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/Universal/${ARCH}/*.lib")

target_link_libraries(CrossGLDX PRIVATE 
    $<$<CONFIG:Debug>:${EXTERNAL_DLIBS}>
    $<$<CONFIG:Release>:${EXTERNAL_RLIBS}>
    ${EXTERNAL_ULIBS}
)

find_package(OpenGL REQUIRED)
target_link_libraries(CrossGLDX PRIVATE OpenGL::GL)

if(WIN32)
    target_link_libraries(CrossGLDX PRIVATE d3d11 dxgi d3dcompiler)
endif()

# Build Test
add_executable(BuildTest tests/BuildTest.cpp)
target_link_libraries(BuildTest PRIVATE CrossGLDX)

# Definitions
target_compile_definitions(CrossGLDX PUBLIC 
    $<$<CONFIG:Debug>:CrossGLDX_BUILD_DEBUG>
    $<$<CONFIG:Release>:CrossGLDX_BUILD_RELEASE>
)

# Post-Build DLL Copy
set(DLL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/dll/$<CONFIG>/${ARCH}")

if(WIN32)
    add_custom_command(
        TARGET BuildTest
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DLL_SOURCE_DIR}"
            "$<TARGET_FILE_DIR:BuildTest>"
        COMMENT "Copying Runtime DLLs to execution directory..."
        COMMAND_EXPAND_LISTS
    )
endif()