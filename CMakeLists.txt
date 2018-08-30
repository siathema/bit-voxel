cmake_minimum_required (VERSION 2.6)
project(SideMOBA)

if(UNIX AND NOT APPLE)
        set(LINUX TRUE)
endif()

if(LINUX)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)

    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    set(CMAKE_CXX_FLAGS "-g -std=c++11 -fcolor-diagnostics")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else() #MSVC
	set(CMAKE_CXX_FLAGS "-DWIN32=1 -DWIN64=1 /EHsc /Zi /MP ") #/F NUM to change default stack size.
	set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od")
endif()

set(SOURCES
	src/platform_sdl.cpp
	src/renderer.cpp
	src/assets.cpp
    src/mesh.cpp
	src/camera.cpp
	src/sMath.cpp
    src/voxel.cpp
    src/ui.cpp
    src/shared.cpp)

add_executable(client ${SOURCES})

if(WIN32)
	target_include_directories( client PRIVATE
			    				libs/glew-2.1.0/include
			   					libs/SDL2-2.0.8/include
								 )

	target_link_libraries( client
	opengl32.lib
	${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.1.0/lib/Release/x64/glew32.lib
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.0.8/lib/x64/SDL2.lib
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.0.8/lib/x64/SDL2main.lib)
elseif(LINUX)
    target_link_libraries( client
                           ${OPENGL_LIBRARIES}
                           ${SDL2_LIBRARY}
                           pthread
                           GLEW
                           SDL2)
endif()
