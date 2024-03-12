cmake_minimum_required( VERSION 3.22.0)

project(
    HD-VS
    VERSION 2.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Retrieving packages")

find_package(wxWidgets REQUIRED COMPONENTS net core base)
if (wxWidgets_USE_FILE) # not defined in config mode
    include(${wxWidgets_USE_FILE})
endif()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/hdvs.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
