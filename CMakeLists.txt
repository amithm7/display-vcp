cmake_minimum_required(VERSION 3.16)
project(DisplayVCPTray)

set(target_name display-vcp-tray)
set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# find_package(KF6 REQUIRED COMPONENTS CoreAddons StatusNotifierItem)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6StatusNotifierItem REQUIRED)

# Enable AUTOMOC for Qt meta-object compilation
set(CMAKE_AUTOMOC ON)

include_directories(
    # for every new directory with header files
    ${PROJECT_SOURCE_DIR}/src/core
)

add_executable(${target_name}
    # for every new source file (cpp) file
    src/main.cpp
    src/core/ddcutil-wrapper.cpp
)

target_link_libraries(${target_name}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    KF6::CoreAddons
    KF6::StatusNotifierItem
)