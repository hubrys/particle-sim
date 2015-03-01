project(particle_sim)
cmake_minimum_required(VERSION 3.0.2)

option(CudaSupport "Build with support for CUDA" OFF)

#initial setup
if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	add_definitions(-D_DEBUG)
endif()

if(NOT MSVC)
	set(CMAKE_CXX_COMPILER_INIT clang)
endif()


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(ext-deps/glfw-3.1)

include_directories(
	GLEW_INCLUDE_DIRS
	ext-deps/glfw-3.1/include
	ext-deps/glm
)

set(PARTICLE_LIBS
	${GLEW_LIBRARIES}
	glfw
	${GLFW_LIBRARIES})

#Options

set(PARTICLE_SRC	
	source/main.cpp
	source/Config.cpp
	source/Simulation.cpp
	source/rendering/Program.cpp
	source/rendering/Shader.cpp
	source/particle/CpuParticleManager.cpp)

add_executable(particle_sim ${PARTICLE_SRC})
target_link_libraries(particle_sim ${PARTICLE_LIBS})
if(NOT MSVC)
	set_target_properties(particle_sim PROPERTIES
  		COMPILE_FLAGS "-std=c++11"
	)
endif()

configure_file(assets/config.txt assets/config.txt NEWLINE_STYLE UNIX)
configure_file(assets/shaders/simulation.vert assets/shaders/simulation.vert NEWLINE_STYLE UNIX)
configure_file(assets/shaders/simulation.frag assets/shaders/simulation.frag NEWLINE_STYLE UNIX)

source_group(source FILES ${PARTICLE_SRC})