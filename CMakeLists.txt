cmake_minimum_required(VERSION 3.0...3.10)
project(LECTranslate)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_SHARED_MODULE_PREFIX "")

include_directories(
	src
)

add_library(LECTranslate MODULE
	src/lectranslate.cpp
	src/extensionimpl.cpp
)

set_property(TARGET LECTranslate PROPERTY CXX_STANDARD 17)

if(WIN32)
	target_compile_definitions(LECTranslate PUBLIC UNICODE _UNICODE)
	target_link_options(LECTranslate PUBLIC -static -static-libgcc -static-libstdc++)
endif()
