#pragma once

#include <iostream>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "LightPoint.h"
#include "LightDirectional.h"
#include "LightSpot.h"
#include "Cube.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Scene
{
private:
	vector<Light*> lights;
	vector<Camera*> cameras;
	vector<Cube*> cubes;
	vector<Model*> modelsTex;
	vector<Model*> modelsCol;

	Camera* active_camera;

public:
	bool dayNight = true;

	Scene() { lights = vector<Light*>(); cameras = vector<Camera*>(); cubes = vector<Cube*>(); };
	void AddLight(Light* light) { lights.push_back(light); }
	void AddCamera(Camera* camera) { cameras.push_back(camera); }
	void AddCube(Cube* cube) { cubes.push_back(cube); }
	void SetActiveCamera(int index)
	{
		for (Camera* camera: cameras) {
			camera->SetActive(false);
		}
		cameras[index]->SetActive(true);
		active_camera = cameras[index];
	}
	Camera& GetActiveCamera()
	{
		for (Camera* camera : cameras) {
			if (camera->IsActive())
			{
				active_camera = camera;
				break;
			}
		}
		return *active_camera;
	}

	void Update(float deltaTime)
	{
		for (Cube* cube : cubes) {
			cube->UpdatePosition(deltaTime);
		}
		for (Light* light : lights) {
			if (light->GetType() != LightType::DIRECTIONAL)
				continue;
			if (dayNight)
			{
				light->ambient = glm::vec3(0.0f);
				light->diffuse = glm::vec3(0.0f);
				light->specular = glm::vec3(0.0f);
			}
			else
			{
				light->ambient = glm::vec3(0.05f);
				light->diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
				light->specular = glm::vec3(0.6f, 0.6f, 0.6f);
			}
		}
	}

	glm::mat4 GetViewMatrix()
	{
		return active_camera->GetViewMatrix();
	}

	glm::mat4 GetProjectionMatrix()
	{
		return glm::perspective(glm::radians(active_camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	}

	void DrawCubes(Shader& shader, unsigned int& cubeVAO)
	{
		shader.use();

		shader.setMat4("projection", GetProjectionMatrix());
		shader.setMat4("view", GetViewMatrix());
		shader.setVec3("viewPos", active_camera->Position);

		for (Cube* cube: cubes)
		{
			shader.setMat4("model", cube->GetModelMatrix());
			shader.setVec3("objectColor", cube->GetColor());

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	void DrawModels(Shader& shaderTex, Shader& shaderCol)
	{
		for (Model* model : modelsTex)
		{
			DrawModel(shaderTex, *model);
		}
		for (Model* model : modelsCol)
		{
			DrawModel(shaderCol, *model);
		}
	}

	void DrawModel(Shader& shader, Model& model)
	{
		shader.use();
		shader.setMat4("projection", GetProjectionMatrix());
		shader.setMat4("view", GetViewMatrix());
		shader.setVec3("viewPos", active_camera->Position);
		shader.setVec3("objectColor", model.color);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, model.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model.scale);
		shader.setMat4("model", modelMatrix);

		model.Draw(shader);
	}

	
	void AddTextureModel(Model* model) { modelsTex.push_back(model); }
	void AddColorModel(Model* model) { modelsCol.push_back(model); }

	void CreateObjects()
	{
		CreateCameras();
		CreateLights();
		CreateModels();
		CreateCubes();
	}
	void CreateModels()
	{
		//Model ourModel("C:/Users/pietr/Desktop/city/uploads_files_2720101_BusGameMap.obj");
		Model* spider= new Model("C:/Users/pietr/Downloads/spider/spider.obj", glm::vec3(0, 0, 0), 0.05, glm::vec3(1, 1, 1));
		AddTextureModel(spider);


		Model* car = new Model("C:/Users/pietr/Downloads/ferrari-288-gto/source/ferrari 288 gto/ferrari 288 gto.obj", glm::vec3(0, 0, 0), 0.5, glm::vec3(1, 1, 0));
		AddColorModel(car);

		//Model tank("C:/Users/pietr/Downloads/challenger-1-main-battle-tank/source/Challenger 1 Main Battle Tank/Challenger 1 Main Battle Tank.obj",
			//glm::vec3(0, 0, 0), 0.1, glm::vec3(0.4, 1, 0.4), glm::vec3(-90, 0, 0));
		//AddColorModel(&tank);
	}
	void CreateLights()
	{
		Light* light1 = new LightPoint(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
			1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
		AddLight(light1);

		Light* light2 = new LightPoint(glm::vec3(10.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
			1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
		AddLight(light2);

		Light* light3 = new LightDirectional(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, -1, 0),
			glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(1.0f, 1.0f, 1.0f));
		AddLight(light3);

		Light* light4 = new LightDirectional(glm::vec3(-4.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1, -1, 0),
			glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(1.0f, 1.0f, 1.0f));
		AddLight(light4);

		Light* light5 = new LightSpot(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))
			, glm::vec3(0, 0, -1),
			glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.0f, 1.0f, 1.0f));
		AddLight(light5);
	}
	void CreateCameras()
	{
		Camera* camera1= new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		Camera* camera2 = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
		Camera* camera3= new Camera(glm::vec3(0.0f, 0.0f, 30.0f));



		AddCamera(camera1);
		AddCamera(camera2);
		AddCamera(camera3);
		
	}
	void CreateCubes()
	{
		// create a few cubes
		for (int i = 0; i < 20; i++)
		{
			glm::vec3 position = glm::vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
			glm::vec3 scale = glm::vec3(rand() % 20 / 10.0f + 0.2f, rand() % 20 / 10.0f + 0.2f, rand() % 20 / 10.0f + 0.2f);
			//glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
			glm::vec3 color = glm::vec3((rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f);
			//glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
			glm::vec3 rotation = glm::vec3(rand() % 360, rand() % 360, rand() % 360);
			Cube* cube = new Cube(position, scale, color, rotation);
			AddCube(cube);
		}
		cubes[0]->SetVelocity(glm::vec3(1.0f, 0.3f, 3.0f));
	}


	void DrawLights(Shader& shader, unsigned int& lightVAO)
	{
		shader.use();
		
		shader.setMat4("projection", GetProjectionMatrix());
		shader.setMat4("view", GetViewMatrix());
		
		for (Light* light : lights) {
			if (light->GetType() != LightType::POINT)
				continue;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light->GetPosition());
			model = glm::scale(model, glm::vec3(0.2f));
			shader.setMat4("model", model);

			shader.setVec3("lightColor", light->GetColor());

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	vector<Cube*> GetCubes() { return cubes; }
	vector<Light*> GetLights() { return lights; }
	vector<Camera*> GetCameras() { return cameras; }

	LightBuffer LoadLights() {
		LightBuffer lightBuffer;
		lightBuffer.NR_DIR_LIGHTS = 0;
		lightBuffer.NR_POINT_LIGHTS = 0;
		lightBuffer.NR_SPOT_LIGHTS = 0;
		for (Light* light : lights) {
			light->AddTo(lightBuffer);
		}
		return lightBuffer;
	}
};

