#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

char* read_shader(char *shader_path) {

	FILE *shader = fopen(shader_path, "r");
	if(shader == NULL) {
		perror("Error opening shader.\n");
		return "";
	}

	fseek(shader, 0, SEEK_END);
	long file_size = ftell(shader);
	fseek(shader, 0, SEEK_SET);

	char *buffer = (char*)malloc(file_size);

	fread(buffer, file_size, 1, shader);

	fclose(shader);

	return buffer;
}

GLuint compile_shader(GLenum type, const char* source_code) {

	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &source_code, NULL);
	glCompileShader(shader);


	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		perror("Shader compilation error.\n");
		glDeleteShader(shader);
		return 0;
	}

	return shader;

}

GLuint create_shader_program(char* vertex_path, char* fragment_path) {
	const char* vertex_code = read_shader(vertex_path);
	const char* fragment_code = read_shader(fragment_path);

	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_code);
	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_code);

	free((void*)vertex_code);
	free((void*)fragment_code);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint success;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Shader program linking error: %s\n", info_log);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

void draw_quad(GLFWwindow *window) {

	const float size = 0.5f;

	glBegin(GL_QUADS);

	glColor3f(1.0, 0.0, 1.0);
	glVertex2f(-1.0, -1.0);

	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(-1.0,  1.0);
	
	glColor3f(0.0, 0.0, 1.0);
	glVertex2f( 1.0,  1.0);
	
	glColor3f(1.0, 1.0, 0.0);
	glVertex2f( 1.0, -1.0);
	
	glEnd();

}

GLFWwindow* create_window() {

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	GLFWwindow *window = glfwCreateWindow(1920, 1080, "OpenGL Window", monitor, NULL);
	if (!window) {
		glfwTerminate();
		return NULL;
	}

	return window;
}

void setup_glfw(GLFWwindow *window) {

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void setup_glew() {

	if(glewInit() != GLEW_OK) {
		perror("Error initializing GLEW.\n");
		glfwTerminate();
		return;
	}

	if (!GLEW_VERSION_3_3) {
		perror("OpenGL 3.3 is not supported!\n");
		glfwTerminate();
		return;
	}

}

void handle_input(GLFWwindow* window, double* camera_position, double* camera_direction, double* last_mouse_position) {

	// Quit on esc
	if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}


	const double camera_speed = 0.05;
	const double mouse_sensitivity = 0.005;

	double move_vector[] = {0.0, 0.0};

	// Handle key inputs
	if(glfwGetKey(window, GLFW_KEY_W)) {
		move_vector[1] += 1.0;
	}
	if(glfwGetKey(window, GLFW_KEY_A)) {
		move_vector[0] -= 1.0;
	}
	if(glfwGetKey(window, GLFW_KEY_S)) {
		move_vector[1] -= 1.0;
	}
	if(glfwGetKey(window, GLFW_KEY_D)) {
		move_vector[0] += 1.0;
	}

	// Normalize move vector
	double move_magnitude = sqrt(move_vector[0]*move_vector[0] + move_vector[1]*move_vector[1]);
	if(move_magnitude > 0) {
		move_vector[0] *= camera_speed / move_magnitude;
		move_vector[1] *= camera_speed / move_magnitude;
	}

	// Update camera position
	camera_position[0] += move_vector[0];
	camera_position[2] += move_vector[1];

	// Handle mouse inputs
	double current_mouse_position[2];
	glfwGetCursorPos(window, &current_mouse_position[0], &current_mouse_position[1]);

	double mouse_position_difference[] = {
		current_mouse_position[0] - last_mouse_position[0], 
		current_mouse_position[1] - last_mouse_position[1]
	};
	
	camera_direction[0] -= mouse_sensitivity * mouse_position_difference[0];

	



}

int main(int argc, char **argv) {

	if(!glfwInit()) {
		return -1;
	}

	GLFWwindow *window = create_window();
	setup_glfw(window);
	setup_glew();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLuint shader_program = create_shader_program("../src/vertex_shader.glsl", "../src/fragment_shader.glsl");

	float vertices[] = {
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f,  -1.0f, 0.0f,

		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glUseProgram(shader_program);
	GLint resolution_location = glGetUniformLocation(shader_program, "resolution");
	glUniform2f(resolution_location, 1920.0f, 1080.0f);

	double time = 0.0f;
	double camera_position[] = {0.0, 1.0, 0.0}; 
	double camera_direction[] = {0.0, 0.0};
	double last_mouse_position[2];
	glfwGetCursorPos(window, &last_mouse_position[0], &last_mouse_position[1]);

	while (!glfwWindowShouldClose(window)) {

		GLint time_location = glGetUniformLocation(shader_program, "time");
		glUniform1f(time_location, time);
		time += 0.01;

		GLint camera_position_location = glGetUniformLocation(shader_program, "camera_position");
		glUniform3f(camera_position_location, camera_position[0], camera_position[1], camera_position[2]);

		GLint camera_direction_location = glGetUniformLocation(shader_program, "camera_direction");
		glUniform2f(camera_direction_location, camera_direction[0], camera_direction[1]);

		handle_input(window, camera_position, camera_direction, last_mouse_position);
		glfwGetCursorPos(window, &last_mouse_position[0], &last_mouse_position[1]);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		glfwSwapBuffers(window);

		glfwPollEvents();
		
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}
