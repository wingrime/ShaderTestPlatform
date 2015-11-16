cmake_minimum_required(VERSION 2.8)
project (MProject)

set(BUILD_OUTDIR,"${PROJECT_SOURCE_DIR}/Build")
set(EXECUTABLE_OUTPUT_PATH,"${PROJECT_SOURCE_DIR}/Build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

option(DEBUG_BUILD "Build test programs" ON)
if(DEBUG_BUILD)
#set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_CXX_FLAGS "-std=c++11 -static -static-libstdc++ -g -ggdb -gdwarf-2 -O0 -static-libstdc++ -Wall -Wextra -Wl,-Bstatic -lwinpthread  ${CMAKE_CXX_FLAGS}")
else()
add_definitions(-D__BUILD_DROP_ASSERT__)
set(CMAKE_CXX_FLAGS "-std=c++11 -static -static-libstdc++ -static-libstdc++ -Wl,-Bstatic -lwinpthread -O3 -flto -msse2  -mfpmath=sse,387 -msse -fomit-frame-pointer ${CMAKE_CXX_FLAGS}")

#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


if(NOT DEFINED ENV{FREETYPE_DIR})
	set(ENV{FREETYPE_DIR} "${PROJECT_SOURCE_DIR}/External/freetype")
endif()

#to do try fix find
set(GLEW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/External/glew/include")
set(GLEW_LIBRARY "${PROJECT_SOURCE_DIR}/External/glew/lib")
#to do try fix find
set(FREEGLUT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/External/freeglut/freeglut/freeglut/include")
set(FREEGLUT_LIBRARY "${PROJECT_SOURCE_DIR}/External/freeglut/freeglut/freeglut/lib")
#lua
set(LUA_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/External/lua/src")
set(LUA_LIBRARIES "${PROJECT_SOURCE_DIR}/External/lua")


find_package(Freetype REQUIRED)	
find_package(glew REQUIRED)	
find_package(freeglut REQUIRED)
find_package(lua REQUIRED)

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
if(LUA_FOUND)
                include_directories(${LUA_INCLUDE_DIR})
                list(APPEND CORE_LINK_LIBS ${LUA_LIBRARIES})
endif()
#include only library
# cereal
include_directories(${PROJECT_SOURCE_DIR}/External/cereal/include)
#lua config
include_directories(${PROJECT_SOURCE_DIR}/External/lua/)
#selene
include_directories(${PROJECT_SOURCE_DIR}/External/selene/include/)


# gtest
include_directories(${PROJECT_SOURCE_DIR}/External/gtest/include)
# for gtest-all.cc
include_directories(${PROJECT_SOURCE_DIR}/External/gtest/)

#add to "-I"
include_directories(${PROJECT_SOURCE_DIR}/Sources)

include_directories(${PROJECT_SOURCE_DIR}/Sources/Tests)

# imGui
include_directories(${PROJECT_SOURCE_DIR}/External/imgui)

#add to deps
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/Sources RENDER_HEADERS) 

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/Sources/Tests RENDER_TESTS)

#Scripting system
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/Scripting SCRIPTING)

#buildin lua, submodules are strange things there
include_directories(${PROJECT_SOURCE_DIR}/External/lua/src)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/External/lua/src LUA_BUILDIN_SRC)
list(REMOVE_ITEM LUA_BUILDIN_SRC ${PROJECT_SOURCE_DIR}/External/lua/src/luac.c)
list(REMOVE_ITEM LUA_BUILDIN_SRC ${PROJECT_SOURCE_DIR}/External/lua/src/lua.c)
list(REMOVE_ITEM LUA_BUILDIN_SRC ${PROJECT_SOURCE_DIR}/External/lua/src/loadlib_rel.c)

#add git revision to variable
exec_program(
    "git"
    ARGS "rev-parse HEAD"
    OUTPUT_VARIABLE VERSION )


add_definitions( -DGIT_SHA1="${VERSION}" )
message ("${VERSION}")
message ("GIT OUTPUT")

set(CORE_SRC External/imgui/imgui.cpp Scene.cpp DebugUI.cpp IMGuiHooks.cpp  )

add_executable("main" main.cpp ${CORE_SRC}  ${RENDER_HEADERS} ${LUA_BUILDIN_SRC}  )

target_link_libraries(main "${GLEW_LIBRARY}/liblibglew_shared.dll.a")
target_link_libraries(main "${FREEGLUT_LIBRARY}/libfreeglut.dll.a")
target_link_libraries(main OpenGL32)
target_link_libraries(main "${FREETYPE_LIBRARY}")
#target_link_libraries(main "${LUA_LIBRARIES}/liblua.dll.a" ) #build in lua

set_target_properties( main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Build")

add_custom_command(TARGET main
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy "${FREEGLUT_LIBRARY}/../bin/libfreeglut.dll" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy "$ENV{FREETYPE_DIR}/bin/freetype6.dll" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy "${GLEW_LIBRARY}/../bin/libglew.dll" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/PreBuild" "${PROJECT_SOURCE_DIR}/Build/"
          COMMAND ${CMAKE_COMMAND} -E copy "${LUA_LIBRARIES}/liblua.dll" "${PROJECT_SOURCE_DIR}/Build/"   #to link lua
          )
add_executable("tests" ${CORE_SRC} ${LUA_BUILDIN_SRC} "${PROJECT_SOURCE_DIR}/External/gtest/src/gtest_main.cc" "${PROJECT_SOURCE_DIR}/External/gtest/src/gtest-all.cc" "${RENDER_TESTS}" "${RENDER_HEADERS}")
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Build")
target_link_libraries(tests "${GLEW_LIBRARY}/liblibglew_shared.dll.a")
target_link_libraries(tests "${FREEGLUT_LIBRARY}/libfreeglut.dll.a")
target_link_libraries(tests OpenGL32)
target_link_libraries(tests "${FREETYPE_LIBRARY}")
#run tests
add_custom_command(TARGET tests
          POST_BUILD
          COMMAND "${PROJECT_SOURCE_DIR}/Build/tests.exe"
          WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/Build"
          )
