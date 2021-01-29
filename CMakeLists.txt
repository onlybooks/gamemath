##########################################################
## CMake Setting for SoftRenderer 
##########################################################

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakeDependentOption)
set(MAIN_PROJECT_NAME SoftRenderer8-4)

project(${MAIN_PROJECT_NAME})

set(RUNTIME_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime)

if(NOT WIN32)
	message(FATAL_ERROR "This project only supports windows environment currently.")
endif()

if(NOT MSVC)
	message(FATAL_ERROR "This project only supports MSVC project currently.")
endif()

set(PLATFORM_DEFINITION PLATFORM_GENERIC)
set(PLATFORM_FOLDER Generic)

option(TARGET_WINDOWS "Windows Option" ON)

if(TARGET_WINDOWS)
	set(PLATFORM_DEFINITION PLATFORM_WINDOWS)
	set(PLATFORM_FOLDER Windows)
endif()

################################################################################
# Sub projects
################################################################################

add_subdirectory(Source/Runtime/Math)
add_subdirectory(Source/Runtime/Renderer)
add_subdirectory(Source/Runtime/Engine)
add_subdirectory(Source/Player)

################################################################################
# Definitions
################################################################################
target_compile_definitions(SoftRendererPlayer PUBLIC ${PLATFORM_DEFINITION} ${PLATFORM_FOLDER})
target_compile_definitions(EngineModule PUBLIC ${PLATFORM_DEFINITION} ${PLATFORM_FOLDER})
target_compile_definitions(RendererModule PUBLIC ${PLATFORM_DEFINITION} ${PLATFORM_FOLDER})
target_compile_definitions(MathModule PUBLIC ${PLATFORM_DEFINITION} ${PLATFORM_FOLDER})

################################################################################
# Solution Setting
################################################################################
set(CMAKE_SUPPRESS_REGENERATION true)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SoftRendererPlayer)

