cmake_minimum_required(VERSION 3.10.2)
project(MessagesQueue LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(core)
find_package(Qt5 COMPONENTS Core Quick REQUIRED)
add_executable(${PROJECT_NAME}  main.cpp
                                mediator.cpp
                                reader.cpp
                                qml/qml.qrc)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick MessagesQueueCore)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/core)
