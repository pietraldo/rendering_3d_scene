#pragma once

#include <iostream>
#include <vector>

#include "Camera.h"
#include "Light.h"
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
	vector<Model*> models;

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

	void DrawModels(Shader& shader)
	{
		for (Model* model : models) {
			model->Draw(shader);
		}
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

