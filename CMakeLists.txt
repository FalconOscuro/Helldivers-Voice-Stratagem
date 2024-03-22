cmake_minimum_required( VERSION 3.22.0)

project(
    HD-VS
    VERSION 2.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

message(STATUS "Retrieving packages")

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

include(FetchContent)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0
)
FetchContent_GetProperties(yaml-cpp)

if(NOT yaml-cpp_POPULATED)
    message(STATUS "Fetching yaml-cpp...")
    FetchContent_Populate(yaml-cpp)
    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/HDVS)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/mainwindow.cpp
    ${INC_DIR}/mainwindow.h
    ${SRC_DIR}/hdvs.cpp
    ${INC_DIR}/hdvs.h
    ${SRC_DIR}/options.cpp
    ${SRC_DIR}/status.cpp
    ${INC_DIR}/status.h
    ${SRC_DIR}/stratagem.cpp
    ${INC_DIR}/stratagem.h
    ${SRC_DIR}/config.cpp
    ${SRC_DIR}/collapsible.cpp
    ${INC_DIR}/collapsible.h
    ${SRC_DIR}/stratpane.cpp
    ${INC_DIR}/stratpane.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    yaml-cpp::yaml-cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()
