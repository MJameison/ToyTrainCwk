cmake_minimum_required(VERSION 3.2)
project(ToyTrain)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(libraries/assimp)

file(GLOB PROJECT_SOURCE_FILES src/*.cpp)
file(GLOB PROJECT_HEADER_FILES src/*.h)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${GLUT_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	libraries/glm
	libraries/assimp/include
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} assimp)