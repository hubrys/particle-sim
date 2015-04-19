project(particle_sim)
cmake_minimum_required(VERSION 2.8.12)

option(CudaSupport "Build with support for CUDA" OFF)

#initial setup
if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	add_definitions(-D_DEBUG)
endif()

if (MSVC)
	set(GLEW_INCLUDE_DIRS ./ext-deps/glew-1.12.0/include)
	set(GLEW_LIBRARIES ${CMAKE_SOURCE_DIR}/ext-deps/glew-1.12.0/lib/Release/x64/glew32.lib)
	set(GLEW_DLL ${CMAKE_SOURCE_DIR}/ext-deps/glew-1.12.0/bin/Release/x64/glew32.dll)
else (MSVC)
	set(CMAKE_CXX_COMPILER_INIT clang)
	find_package(GLEW REQUIRED)
endif()

find_package(OpenGL REQUIRED)

add_subdirectory(ext-deps/glfw-3.1)

include_directories(
	${GLEW_INCLUDE_DIRS}
	ext-deps/glfw-3.1/include
	ext-deps/glm
)

set(PARTICLE_LIBS
	${GLEW_LIBRARIES}
	glfw
	${GLFW_LIBRARIES})

set(PARTICLE_SRC	
	source/main.cpp
	source/Config.h
	source/Config.cpp
	source/Simulation.h
	source/Simulation.cpp
	)

set(PARTICLE_SRC_RENDERING
	source/rendering/Program.h
	source/rendering/Program.cpp
	source/rendering/Shader.h
	source/rendering/Shader.cpp
	)

set(PARTICLE_SRC_PARTICLE
	source/particle/CpuParticleManager.h
	source/particle/CpuParticleManager.cpp
	source/particle/SteppedCpuParticleManager.h
	source/particle/SteppedCpuParticleManager.cpp
	)

set(PARTICLE_ASSETS
	assets/config.txt
	assets/shaders/simulation.vert
	assets/shaders/colorByNone.frag
	assets/shaders/colorByDirection.frag
	assets/shaders/colorByVelocity.frag
	assets/shaders/colorByBoth.frag
	)

if (MSVC)
	add_executable(particle_sim 
		${PARTICLE_SRC}
		${PARTICLE_SRC_RENDERING}
		${PARTICLE_SRC_PARTICLE}
		${PARTICLE_ASSETS}
		)
else (MSVC)
	add_executable(particle_sim 
		${PARTICLE_SRC}
		${PARTICLE_SRC_RENDERING}
		${PARTICLE_SRC_PARTICLE}
		)
endif (MSVC)

target_link_libraries(particle_sim ${PARTICLE_LIBS})

if(NOT MSVC)
	set_target_properties(particle_sim PROPERTIES
  		COMPILE_FLAGS "-std=c++11"
	)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${GLEW_INCLUDE_DIRS}")
endif()

source_group(source REGULAR_EXPRESSION "source/.*")
source_group(source\\rendering REGULAR_EXPRESSION "source/rendering/.*")
source_group(source\\particle REGULAR_EXPRESSION "source/particle/.*")
source_group(assets REGULAR_EXPRESSION "assets/.*")
source_group(assets\\shaders REGULAR_EXPRESSION "assets/shaders/.*")

add_custom_command(TARGET particle_sim POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${GLEW_DLL} 
                       ${CMAKE_BINARY_DIR})

foreach (FILE ${PARTICLE_ASSETS})
	add_custom_command(TARGET particle_sim PRE_BUILD
    	               COMMAND ${CMAKE_COMMAND} -E copy_if_different
        	               ${CMAKE_SOURCE_DIR}/${FILE}
            	           ${CMAKE_BINARY_DIR}/${FILE})
endforeach (FILE)