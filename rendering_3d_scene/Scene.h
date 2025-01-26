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
#include "Sphere.h"

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
	vector<Sphere*> spheres;
	vector<Model*> modelsTex;
	vector<Model*> modelsCol;

	Camera* active_camera;

	void CreateModels();
	void CreateLights();
	void CreateCameras();
	void CreateCubes();
	void CreateSpheres();

public:
	bool dayNight = true;
	bool Gouraud = false;
	bool fog = false;
	bool userFlashlight = false;

	LightSpot* flashlight;

	Scene() { lights = vector<Light*>(); cameras = vector<Camera*>(); cubes = vector<Cube*>(); };
	void AddLight(Light* light) { lights.push_back(light); }
	void AddCamera(Camera* camera) { cameras.push_back(camera); }
	void AddCube(Cube* cube) { cubes.push_back(cube); }
	void AddSphere(Sphere* sphere) { spheres.push_back(sphere); }
	void SetActiveCamera(int index);
	Camera& Scene::GetActiveCamera();
	void Update(float deltaTime);
	void UpdateFlashLight()
	{
		if (userFlashlight)
		{
			flashlight->specular = glm::vec3(1.0f);
			flashlight->diffuse = glm::vec3(0.6f);
			flashlight->ambient = glm::vec3(0.0f);
		}
		else
		{
			flashlight->specular = glm::vec3(0.0f);
			flashlight->diffuse = glm::vec3(0.0f);
			flashlight->ambient = glm::vec3(0.0f);
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

	void DrawCubes(Shader& shader, unsigned int& cubeVAO);

	void DrawSpheres(Shader& shader, unsigned int& sphereVAO);

	void DrawModels(Shader& shaderTex, Shader& shaderCol);

	void DrawModel(Shader& shader, Model& model);
	
	void AddTextureModel(Model* model) { modelsTex.push_back(model); }
	void AddColorModel(Model* model) { modelsCol.push_back(model); }

	void CreateObjects();

	

	void DrawLights(Shader& shader, unsigned int& lightVAO);

	vector<Cube*> GetCubes() { return cubes; }
	vector<Sphere*> GetSpheres() { return spheres; }
	vector<Light*> GetLights() { return lights; }
	vector<Camera*> GetCameras() { return cameras; }

	LightBuffer LoadLights();
};

