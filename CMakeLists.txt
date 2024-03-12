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
set(CMAKE_AUTORRC ON)
set(CMAKE_AUTOUIC ON)

message(STATUS "Retrieving packages")

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/hdvs.cpp
    ${SRC_DIR}/options.cpp
    ${SRC_DIR}/status.cpp
    ${SRC_DIR}/stratagem.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt::Widgets
    Qt::Core
    Qt::Gui
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
