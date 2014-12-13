#TODO LIST
# fix zlib
# deal with static/dynamic link

cmake_minimum_required(VERSION 2.8)



project (m_proj)

set(BUILD_OUTDIR,"${PROJECT_SOURCE_DIR}/Build")


set (CMAKE_CXX_COMPILER "g++")
set (CMAKE_C_COMPILER "gcc") 
set (CMAKE_RC_COMPILER "windres") 
set (CMAKE_MAKE_PROGRAM "wingw32-make") 
set (CMAKE_BUILD_TOOL "wingw32-make") 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

option(DEBUG_BUILD "Build test programs" ON)
if(DEBUG_BUILD)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_CXX_FLAGS "-std=c++11 -static -static-libstdc++ -g -ggdb -static-libstdc++ -Wl,-Bstatic -lwinpthread  ${CMAKE_CXX_FLAGS}")
else()
add_definitions(-D__BUILD_DROP_ASSERT__)
set(CMAKE_CXX_FLAGS "-std=c++11 -static -static-libstdc++ -static-libstdc++ -Wl,-Bstatic -lwinpthread -O3 -flto ${CMAKE_CXX_FLAGS}")
#todo ?
#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")
endif()



set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

message ("M Proj building")

if(NOT DEFINED ENV{FREETYPE_DIR})
	set(ENV{FREETYPE_DIR} "${PROJECT_SOURCE_DIR}/External/freetype")
endif()

#to do try fix find
set(GLEW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/External/glew/include")
set(GLEW_LIBRARY "${PROJECT_SOURCE_DIR}/External/glew/lib")
#to do try fix find
set(FREEGLUT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/External/freeglut/freeglut/freeglut/include")
set(FREEGLUT_LIBRARY "${PROJECT_SOURCE_DIR}/External/freeglut/freeglut/freeglut/lib")

find_package(Freetype REQUIRED)	
find_package(glew REQUIRED)	
find_package(freeglut REQUIRED)	

if(FREETYPE_FOUND)
		include_directories(${FREETYPE_INCLUDE_DIRS})
		link_directories(${FREETYPE_LINK_DIRS})
		list(APPEND CORE_LINK_LIBS ${FREETYPE_LIBRARY})
endif()
if(GLEW_FOUND)
		include_directories(${GLEW_INCLUDE_DIRS})
		link_directories(${GLEW_LINK_DIRS})
		list(APPEND CORE_LINK_LIBS ${GLEW_LIBRARY})
endif()
if(FREEGLUT_FOUND)
		include_directories(${FREEGLUT_INCLUDE_DIRS})
		link_directories(${FREEGLUT_LINK_DIRS})
		list(APPEND CORE_LINK_LIBS ${FREEGLUT_LIBRARY})
endif()

include_directories(${PROJECT_SOURCE_DIR}/External/cereal/include)


#add to "-I"
include_directories(${PROJECT_SOURCE_DIR}/Sources)



#add to deps
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/Sources RENDER_HEADERS) 



message ("${RENDER_HEADERS}")



add_executable("main" main.cpp ${RENDER_HEADERS})
target_link_libraries(main "${GLEW_LIBRARY}/liblibglew_shared.dll.a")
target_link_libraries(main "${FREEGLUT_LIBRARY}/libfreeglut.dll.a")
target_link_libraries(main OpenGL32)
target_link_libraries(main "${FREETYPE_LIBRARY}")
set_target_properties( main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Build")

add_custom_command(TARGET main
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy "${FREEGLUT_LIBRARY}/../bin/libfreeglut.dll" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy "$ENV{FREETYPE_DIR}/bin/freetype6.dll" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy "${GLEW_LIBRARY}/../bin/libglew.dll" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/PreBuild" "${PROJECT_SOURCE_DIR}/Build/"
          )
