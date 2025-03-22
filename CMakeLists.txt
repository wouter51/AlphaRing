cmake_minimum_required(VERSION 3.0)

project(alpha_ring)

# set defitions
set(CMAKE_CXX_STANDARD 20)
set(WRAPPER_NAME WTSAPI32)

# set compiler options
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("-finput-charset=utf-8" "-fexec-charset=utf-8")
else()
    add_compile_options("/utf-8")
endif()
add_compile_definitions(WRAPPER_NAME=${WRAPPER_NAME} PROJECT_NAME=${PROJECT_NAME})

# set dependencies directories
set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/dep/spdlog)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/dep/imgui)
set(LIBMCC_DIR ${CMAKE_SOURCE_DIR}/dep/libmcc)
set(JSON_DIR ${CMAKE_SOURCE_DIR}/dep/json)

# manually add imgui source files
set(IMGUI_SRC 
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/backends/imgui_impl_win32.cpp
	${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

add_library(imgui STATIC ${IMGUI_SRC})

# include headers
include_directories(${SPDLOG_DIR}/include)
include_directories(${IMGUI_DIR})
include_directories(${LIBMCC_DIR}/include)
include_directories(${JSON_DIR}/single_include)

# add source files
file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/src/*.c ${CMAKE_SOURCE_DIR}/src/*.cpp)

# create dll file
add_library(${WRAPPER_NAME} SHARED ${SRC})

# use cinterface and disable d3d11 helpers
target_compile_definitions(${WRAPPER_NAME} PUBLIC CINTERFACE D3D11_NO_HELPERS)
target_compile_options(${WRAPPER_NAME} PRIVATE /wd4002 /wd4005)

# link libraries
target_link_libraries(${WRAPPER_NAME}
	dxguid imgui Synchronization
)

# install 
install(TARGETS ${WRAPPER_NAME} DESTINATION "$ENV{MCC_DIR}/mcc/binaries/win64")
