cmake_minimum_required(VERSION 3.15)
project(bt LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS /utf-8)

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")
if(NOT ${IS_DEBUG_BUILD})
    add_compile_definitions(NOT_DEBUG)
endif()

# glm
add_subdirectory(glm)

# Vulkan
set(Vulkan_SDK "D:/VulkanSDK/1.3.236.0")
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
file(GLOB shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag)
foreach(shader IN LISTS shaders)
    get_filename_component(filename ${shader} NAME ABSOLUTE)
    add_custom_command(
        COMMAND
        ${glslc_executable}
        -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${filename}.spv
        ${shader}
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${filename}.spv
        DEPENDS ${shader} ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        COMMENT "Compiling ${filename}"
    )
    list(APPEND spv_shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${filename}.spv)
endforeach()
add_custom_target(shaders ALL DEPENDS ${spv_shaders})

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(
    "include"
    "include/graphics"
    "include/graphics/vulkan"
    "glm"
    ${Vulkan_INCLUDE_DIRS}
)

add_executable(
    ${PROJECT_NAME}
    WIN32
    ${SRCS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ${Vulkan_LIBRARIES}
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE 
    /EHsc /W4 /WX
)