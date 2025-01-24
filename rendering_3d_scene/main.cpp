
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdlib>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "Light.h"
#include "LightPoint.h"
#include "Cube.h"
#include "Constants.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../externals/stb_image/stb_image.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* CreateWindow(int width, int height, const char* title);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera moving
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct DirLight {
	alignas(16)glm::vec3 direction; // Use glm::vec4 to ensure padding
	alignas(16)glm::vec3 ambient;
	alignas(16)glm::vec3 diffuse;
	alignas(16)glm::vec3 specular;
};

struct PointLight {
	alignas(16)glm::vec3 position;
	alignas(16)glm::vec3 ambient;
	alignas(16)glm::vec3 diffuse;
	alignas(16)glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;

};

struct SpotLight {
	alignas(16)glm::vec3 position;  // Use glm::vec4 to ensure padding
	alignas(16)glm::vec3 direction; // Use glm::vec4 to ensure padding
	
	alignas(16)glm::vec3 ambient;
	alignas(16)glm::vec3 diffuse;
	alignas(16)glm::vec3 specular;

	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

struct LightBuffer {
	DirLight dirLights[MAX_DIR_LIGHTS];
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];
	int NR_DIR_LIGHTS;
	int NR_POINT_LIGHTS;
	int NR_SPOT_LIGHTS;
};


Scene scene;
int main()
{
	srand(19);
	GLFWwindow* window = CreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering 3D scene");
	if (window == nullptr) return -1;

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//stbi_set_flip_vertically_on_load(true);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);


	Shader ourShader("vertex_shader.txt", "fragment_shader.txt");
	Shader lightShader("vertex_shader2.txt", "fragment_shader2.txt");


	Camera camera1(glm::vec3(0.0f, 0.0f, 3.0f));
	Light* light1 = new LightPoint(glm::vec3(1.1f, 2.0f, -3.0f), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, 0.09f, 0.032f);

	// create a few cubes
	for (int i = 0; i < 20; i++)
	{
		glm::vec3 position = glm::vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
		glm::vec3 scale = glm::vec3(rand() % 20 / 10.0f + 0.2f, rand() % 20 / 10.0f + 0.2f, rand() % 20 / 10.0f + 0.2f);
		//glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		//glm::vec3 color = glm::vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 rotation = glm::vec3(rand() % 360, rand() % 360, rand() % 360);
		Cube* cube = new Cube(position, scale, color, rotation);
		scene.AddCube(cube);
	}


	scene.AddLight(light1);
	scene.AddCamera(&camera1);
	scene.SetActiveCamera(0);

	LightBuffer lightBuffer;
	DirLight dirLight;
	dirLight.direction = glm::vec3(0, 1, 0);
	dirLight.ambient = glm::vec3(0.05, 0.05, 0.05);
	dirLight.diffuse = glm::vec3(0.4, 0.4, 0.4);
	dirLight.specular = glm::vec3(0.6, 0.6, 0.6);
	lightBuffer.dirLights[0] = dirLight;
	lightBuffer.NR_DIR_LIGHTS = 1;

	PointLight pointLight;
	pointLight.position = glm::vec3(3, 1, -3);
	pointLight.ambient = glm::vec3(0.0005f, 0.0005f, 0.0005f);
	pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	pointLight.specular = glm::vec3(1.0f, 0.0f, 0.0f);
	pointLight.constant = 0.2f;
	pointLight.linear = 0.09f;
	pointLight.quadratic = 0.05;
	lightBuffer.pointLights[0] = pointLight;
	
	PointLight pointLight2;
	pointLight2.position = glm::vec3(-4, 1, -3);
	pointLight2.ambient = glm::vec3(0.0005f, 0.0005f, 0.0005f);
	pointLight2.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	pointLight2.specular = glm::vec3(1.0f, 0.0f, 0.0f);
	pointLight2.constant = 0.2f;
	pointLight2.linear = 0.09f;
	pointLight2.quadratic = 0.05;
	lightBuffer.pointLights[1] = pointLight2;
	lightBuffer.NR_POINT_LIGHTS = 2;

	SpotLight spotLight;
	spotLight.position = glm::vec3(scene.GetActiveCamera().Position);
	spotLight.direction = glm::vec3(scene.GetActiveCamera().Front);
	spotLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	spotLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	spotLight.specular = glm::vec3(0.6f, 0.6f, 0.6f);
	spotLight.constant = 1.0f;
	spotLight.linear = 0.09f;
	spotLight.quadratic = 0.032f;
	spotLight.cutOff = glm::cos(glm::radians(12.5f));
	spotLight.outerCutOff = glm::cos(glm::radians(15.0f));
	lightBuffer.spotLights[0] = spotLight;
	lightBuffer.NR_SPOT_LIGHTS = 1;
	

	

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube::vertices), Cube::vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(int)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int uniformBlockIndexLights = glGetUniformBlockIndex(ourShader.ID, "Lights");
	glUniformBlockBinding(ourShader.ID, uniformBlockIndexLights, 0);

	unsigned int uboLights;
	glGenBuffers(1, &uboLights);

	glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboLights, 0, sizeof(LightBuffer));

	glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		lightBuffer.spotLights[0].position = glm::vec3(scene.GetActiveCamera().Position);
		lightBuffer.spotLights[0].direction = glm::vec3(scene.GetActiveCamera().Front);
		glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glm::mat4 projection = glm::perspective(glm::radians(scene.GetActiveCamera().Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = scene.GetActiveCamera().GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setVec3("viewPos", scene.GetActiveCamera().Position);




		vector<Cube*> cubes = scene.GetCubes();
		for (int i = 0; i < cubes.size(); i++)
		{
			ourShader.setMat4("model", cubes[i]->GetModelMatrix());

			ourShader.setVec3("objectColor", cubes[i]->GetColor());

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lightShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, scene.GetLights()[0]->GetPosition());
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setMat4("model", model);

		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		lightShader.setVec3("lightColor", scene.GetLights()[0]->GetColor());

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLight2.position);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setMat4("model", model);
		lightShader.setVec3("lightColor", scene.GetLights()[0]->GetColor());

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

GLFWwindow* CreateWindow(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	scene.GetActiveCamera().ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scene.GetActiveCamera().ProcessMouseScroll(static_cast<float>(yoffset));
}