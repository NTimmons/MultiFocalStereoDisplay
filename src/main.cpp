

#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void render(GLFWwindow* _window)
{
	float ratio;
	int width, height;

	glfwMakeContextCurrent(_window);
	glfwGetFramebufferSize(_window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-0.6f, -0.4f, 0.f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.6f, -0.4f, 0.f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.6f, 0.f);
	glEnd();

	glfwSwapBuffers(_window);
	glfwPollEvents();
}

int main(void)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	GLFWwindow* left_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!left_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLFWwindow* right_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!right_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwSwapInterval(2);

	glfwSetKeyCallback(left_window, key_callback);

	while (!glfwWindowShouldClose(left_window) && !glfwWindowShouldClose(right_window))
	{
		render(left_window);
		render(right_window);
	}

	glfwDestroyWindow(left_window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

//! [code]
