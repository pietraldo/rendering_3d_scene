
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
#include "LightDirectional.h"
#include "LightSpot.h"
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
void RenderImGui();



// camera moving
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;




Scene scene;
int main()
{
	srand(19);
	GLFWwindow* window = CreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering 3D scene");
	if (window == nullptr) return -1;




	
	Shader spiderShader("vertex_spider.txt", "fragment_spider.txt");

	Shader ourShader("vertex_shader.txt", "fragment_shader.txt");
	Shader lightShader("vertex_shader2.txt", "fragment_shader2.txt");
	Shader simpleShader("vertex_simple.txt", "fragment_simple.txt");

	

	scene.CreateObjects();
	scene.SetActiveCamera(0);

	float vertices[] = {
	 20,  20, 0.0f,  // top right
	 20, -20, 0.0f,  // bottom right
	-20, -20, 0.0f,  // bottom left
	-20,  20, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	vector<float> vert = { 20,  20, 0.0f,  // top right
		 20, -20, 0.0f,  // bottom right
		-20, -20, 0.0f,  // bottom left
		-20,  20, 0.0f   // top left
	};

	vector<int> ind;
	ind.push_back(0);
	ind.push_back(1);
	ind.push_back(3);
	ind.push_back(1);
	ind.push_back(2);
	ind.push_back(3);

	

	unsigned int VBO2, VAO2, EBO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(float), vert.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*ind.size(), ind.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	LightBuffer lightBuffer = scene.LoadLights();

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

		// updating light buffer
		lightBuffer = scene.LoadLights();
		lightBuffer.spotLights[0].position = glm::vec3(scene.GetActiveCamera().Position);
		lightBuffer.spotLights[0].direction = glm::vec3(scene.GetActiveCamera().Front);

		glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		
		scene.Update(deltaTime);

		// setting cameras
		Cube* cube0 = scene.GetCubes()[0];
		scene.GetCameras()[1]->Front = glm::normalize(cube0->GetPosition() - scene.GetCameras()[1]->Position);
		scene.GetCameras()[2]->Front = glm::normalize(cube0->GetVelocity());
		scene.GetCameras()[2]->Position = cube0->GetPosition() - scene.GetCameras()[2]->Front * 10.0f;

		

		scene.DrawCubes(ourShader, cubeVAO);
		scene.DrawLights(lightShader, lightVAO);

		scene.DrawModels(spiderShader, ourShader);
		

		simpleShader.use();
		simpleShader.setMat4("projection", scene.GetProjectionMatrix());
		simpleShader.setMat4("view", scene.GetViewMatrix());
		simpleShader.setMat4("model", glm::mat4(1.0f));
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0);

		RenderImGui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void RenderImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Camera settings");
		vector<Camera*> cameras = scene.GetCameras();
		for (int i = 0; i < cameras.size(); i++)
		{
			ImGui::Checkbox(("Camera " + to_string(i)).c_str(), &cameras[i]->isActive);
		}
		ImGui::Text("Hello, world!");
		ImGui::End();
	}

	{
		ImGui::Begin("Light settings");
		ImGui::Checkbox("Day/Night", &scene.dayNight);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Initialize backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

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
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	//if right mouse button is pressed, do not move the camera
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	scene.GetActiveCamera().ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scene.GetActiveCamera().ProcessMouseScroll(static_cast<float>(yoffset));
}