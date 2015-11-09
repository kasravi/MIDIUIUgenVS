cmake_minimum_required (VERSION 2.8)
project (MIDIUIUgen)
set(SC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/SuperCollider-Source)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/external_libraries/libsndfile/)

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

add_library(MIDIUIUgen MODULE MIDIUIUgen.cpp)