cmake_minimum_required(VERSION 3.0...3.10)
project(LECTranslate)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(WIN32)
	add_definitions(-static -static-libgcc -static-libstdc++)
endif()

add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_SHARED_MODULE_PREFIX "")

include_directories(
	src
)

add_library(LECTranslate MODULE
	src/lectranslate.cpp
	src/extensionimpl.cpp
)
