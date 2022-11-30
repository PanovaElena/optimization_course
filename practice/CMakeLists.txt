cmake_minimum_required(VERSION 3.1.0)

option(VECTORIZED "vectorized code" OFF)

set(PROJECT "simple_particle_pusher")
project(${PROJECT})

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -qopt-report=3 -qopenmp")
	if (VECTORIZED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xHost -qopt-zmm-usage=high")
	endif()
elseif (WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /debug /O2 /Qopt-report=3 /Qopenmp")
	if (VECTORIZED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /QxHost /Qopt-zmm-usage=high")
	endif()
endif()

file(GLOB MODEL_SRC ${CMAKE_SOURCE_DIR}/src/include/*.h)
add_custom_target(model SOURCES ${MODEL_SRC})
include_directories(src/include/)

file(GLOB TEST_SRC ${CMAKE_SOURCE_DIR}/src/test/*.cpp)
add_executable(test ${TEST_SRC})

if (WIN32)
	file(GLOB VIEW_SRC ${CMAKE_SOURCE_DIR}/src/view/*.cpp)
	add_executable(view ${VIEW_SRC})
endif()
