#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include "Camera.h"
#include "VBO.h"
#include "Shader.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include "Water.h"
#include "UIRenderer.h"

// Meter las cosas en clases

Camera camera = Camera();

float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
float fov = 45.0f;

std::vector<Model*> models;

void mouse_callback
(
	GLFWwindow* window,
	double xpos, double ypos
)
{

	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


/*
	Creo esta clase para poder mezclar los proyectos
	Luego habra que hacer clases para modelos meshes, vao, EBO?

*/
class WallMesh {
public:
	unsigned int vao;
	unsigned int ebo;
	VBO* vbo;

	Shader* shader; // Make a material class

	float angle = 0;

	float *vertices;
	unsigned int* indices;

	WallMesh() 
	{
		// vertices
		// POSxyz, colrgb, uv 
		vertices = new float[32] {
			 0.5f,  0.5f, .0f, 1.0f, .0f, .0f, 1.0f, 1.0f, // top right
			 0.5f, -0.5f, .0f, .0f, 1.0f, .0f, 1.0f, 0.0f,  // bottom right
			-0.5f, -0.5f, .0f, .0f, .0f, 1.0f, 0.0f, 0.0f,// bottom left
			-0.5f,  0.5f, .0f, 1.0f,1.0f,1.0f, 0.0f, 1.0f// top left 
		};
		indices = new unsigned int[6] {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		glGenVertexArrays(1, &vao);
		vbo = new VBO();
		glGenBuffers(1, &ebo);
		
		// 1. bind Vertex Array Object
		glBindVertexArray(vao);
		// 2. copy our vertices array in a vertex buffer for OpenGL to use
		vbo->load(vertices, 32); // Vertices and qty
		// 3. copy our index array in a element buffer for OpenGL to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);
		// 4. then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		// Create shader
		shader = new Shader("shaders/shader.vert", "shaders/shader.frag");

		//Texture *texture_wall = new Texture("container.jpg", "text"false, true);
		//Texture *texture_wall2 = new Texture("roof.jpg", false, true);
		//shader->addTexture(texture_wall, "texture0");
		//shader->addTexture(texture_wall2, "texture1");
	}

	void update(double DeltaTime)
	{
		angle += float(DeltaTime);
	}

	void draw(const Camera &camera)
	{
		shader->use();
		shader->setFloat("time", glfwGetTime());

		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::rotate(model, glm::sin(angle), glm::vec3(1.0, 0.0, 0.0));

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), 640.0f / 480.0f, 0.1f, 100.0f);

		shader->setMat4("model", model);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		glBindVertexArray(vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // cantidad de indices
		glBindVertexArray(0);

	}
};

std::vector<WallMesh*> wallMeshes;


void resizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, const float &deltatime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltatime);
}

void update(double DeltaTime)
{
	camera.updateViewMatrix();
	for (Model* model : models)
	{
		model->update(DeltaTime);
	}
}

void render() 
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Model* model : models)
	{
		model->draw(&camera);
	}

}

int main()
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable mouse
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// Init
	//models.push_back(new Model("assets/que.gltf"));
	models.push_back(new Model("assets/backpack.obj"));
	//wallMeshes.push_back(new WallMesh());

	Light* light = new Light({ 1.f, 1.f, 1.f }, { 3.f, 5.f, 0.f });
	Shader::updateLights();

	Water water;

	Shader* shader = new Shader("shaders/ui.vert", "shaders/ui.frag");

	UIRenderer gui(shader);

	glEnable(GL_DEPTH_TEST);
	auto lastTime = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		auto current = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed = current - lastTime;

		processInput(window, elapsed.count());
		update(elapsed.count());
		glEnable(GL_CLIP_DISTANCE0);
		water.update(models, &camera);
		glDisable(GL_CLIP_DISTANCE0);
		render();
		water.draw(&camera);

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastTime = current;
	}

	glfwTerminate();
	return 0;
}