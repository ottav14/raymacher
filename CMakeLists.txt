cmake_minimum_required(VERSION 3.10)
project(glfw_template)

set(OpenGL_GL_PREFERENCE LEGACY)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(out src/main.c)

target_include_directories(out PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(out PRIVATE 
	glfw 
	GLEW::GLEW 
	m
	${OPENGL_LIBRARIES}
)
