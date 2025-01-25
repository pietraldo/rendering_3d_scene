#pragma once

#include <iostream>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Cube.h"
#include "Model.h"

using namespace std;

class Scene
{
private:
	vector<Light*> lights;
	vector<Camera*> cameras;
	vector<Cube*> cubes;

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

