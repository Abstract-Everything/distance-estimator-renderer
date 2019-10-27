cmake_minimum_required (VERSION 3.10)
project (viewer VERSION 0.1
                DESCRIPTION "A gui for the renderer."
                LANGUAGES CXX)

find_package (Qt5 COMPONENTS Qml Quick REQUIRED)

add_subdirectory (source)

