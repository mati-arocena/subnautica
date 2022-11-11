#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>


void resizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void update(double DeltaTime)
{

}

void render() 
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main()
{
	if (!glfwInit())
		return -1;


	GLFWwindow* window = glfwCreateWindow(640, 480, "Subnautica", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		return -1;

	glfwSetFramebufferSizeCallback(window, resizeWindow);

	auto lastTime = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		auto current = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed = lastTime - current;

		processInput(window);
		update(elapsed.count());
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastTime = current;
	}

	glfwTerminate();
	return 0;
}