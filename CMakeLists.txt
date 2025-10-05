cmake_minimum_required(VERSION 3.16)

project(main)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC
	#self written C++
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/*.h"
	"${CMAKE_SOURCE_DIR}/src/*.hpp"
	"${CMAKE_SOURCE_DIR}/src/*.c"
	#non-compiled library .cpp files
	"${CMAKE_SOURCE_DIR}/libs/header/*.cpp"
	"${CMAKE_SOURCE_DIR}/libs/header/*.c"
)

add_executable(${PROJECT_NAME} ${SRC})

#shortcut macro for asssets
target_compile_definitions(${PROJECT_NAME} PUBLIC
  ASSETS_PATH=\"$<IF:$<CONFIG:Debug>,${CMAKE_CURRENT_SOURCE_DIR}/assets/,./assets/>\"
)

#including the headers
target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_SOURCE_DIR}/libs/header"
	"${CMAKE_SOURCE_DIR}/libs/compiled/SDL2/include"
)

#linking all the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
	"${CMAKE_SOURCE_DIR}/libs/compiled/SDL2/lib/x64/*.lib"
)

#precompiled dynamic library dlls
file(GLOB ALL_DLLS
	"${CMAKE_SOURCE_DIR}/libs/compiled/SDL2/lib/x64/*.dll"
)
#place the dlls next to the executable
foreach(dll IN LISTS ALL_DLLS)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	  "${dll}" $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
endforeach()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/" DESTINATION assets)
install(FILES ${ALL_DLLS} DESTINATION .)
