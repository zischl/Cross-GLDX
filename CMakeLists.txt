cmake_minimum_required(VERSION 3.15)
project(CrossGLDX VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Architecture Detection
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()


include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)

# Don't need no samples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)


# Source Definitions
set(COMMON_SOURCES
    src/Logger.cpp
    src/Logger.h
)

set(WIN32_SOURCES
    src/D3D11Renderer.cpp
    src/D3D11Renderer.h
)

set(GL_SOURCES
    src/GLRenderer.cpp
    src/GLRenderer.h
    external/src/glad.c
)

# Target Setup
if(WIN32)
    set(SOURCES ${COMMON_SOURCES} ${WIN32_SOURCES} ${GL_SOURCES})
else()
    set(SOURCES ${COMMON_SOURCES} ${GL_SOURCES})
endif()

add_library(CrossGLDX STATIC ${SOURCES})
target_include_directories(CrossGLDX PUBLIC src)
target_include_directories(CrossGLDX PUBLIC external/include)

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
add_executable(BuildTest tests/BuildTest.cpp tests/WinForge.cpp)
target_link_libraries(BuildTest PRIVATE CrossGLDX glfw)

# Examples
add_executable(WinApiSync tests/WinApiSync.cpp tests/WinForge.cpp)
target_link_libraries(WinApiSync PRIVATE CrossGLDX glfw)

add_executable(WinApiAsync tests/WinApiAsync.cpp tests/WinForge.cpp)
target_link_libraries(WinApiAsync PRIVATE CrossGLDX glfw)

add_executable(GlfwWindow tests/GlfwWindow.cpp)
target_link_libraries(GlfwWindow PRIVATE CrossGLDX glfw)

# Definitions
target_compile_definitions(CrossGLDX PUBLIC 
    $<$<CONFIG:Debug>:CrossGLDX_BUILD_DEBUG>
    $<$<CONFIG:Release>:CrossGLDX_BUILD_RELEASE>
)

# Post-Build DLL Copy
set(DLL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/dll/$<CONFIG>/${ARCH}")

if(WIN32)
    file(TO_NATIVE_PATH "${DLL_SOURCE_DIR}" NATIVE_DLL_SOURCE_DIR)
    
    add_custom_command(
        TARGET BuildTest
        POST_BUILD
        COMMAND cmd /C "if exist \"${NATIVE_DLL_SOURCE_DIR}\" ${CMAKE_COMMAND} -E copy_directory \"${DLL_SOURCE_DIR}\" \"$<TARGET_FILE_DIR:BuildTest>\""
        COMMENT "Copying Runtime DLLs to execution directory..."
        VERBATIM
    )
    add_custom_command(
        TARGET WinApiSync
        POST_BUILD
        COMMAND cmd /C "if exist \"${NATIVE_DLL_SOURCE_DIR}\" ${CMAKE_COMMAND} -E copy_directory \"${DLL_SOURCE_DIR}\" \"$<TARGET_FILE_DIR:WinApiSync>\""
        COMMENT "Copying Runtime DLLs to execution directory..."
        VERBATIM
    )
    add_custom_command(
        TARGET WinApiAsync
        POST_BUILD
        COMMAND cmd /C "if exist \"${NATIVE_DLL_SOURCE_DIR}\" ${CMAKE_COMMAND} -E copy_directory \"${DLL_SOURCE_DIR}\" \"$<TARGET_FILE_DIR:WinApiAsync>\""
        COMMENT "Copying Runtime DLLs to execution directory..."
        VERBATIM
    )
    add_custom_command(
        TARGET GlfwWindow
        POST_BUILD
        COMMAND cmd /C "if exist \"${NATIVE_DLL_SOURCE_DIR}\" ${CMAKE_COMMAND} -E copy_directory \"${DLL_SOURCE_DIR}\" \"$<TARGET_FILE_DIR:GlfwWindow>\""
        COMMENT "Copying Runtime DLLs to execution directory..."
        VERBATIM
    )
endif()
