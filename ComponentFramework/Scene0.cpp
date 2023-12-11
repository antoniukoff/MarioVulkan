#include <glew.h>
#include <iostream>
#include "Debug.h"
#include "Scene0.h"
#include "MMath.h"
#include "Debug.h"
#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"
#include "Camera.h"


Scene0::Scene0(Renderer *renderer_): 
	Scene(nullptr),renderer(renderer_), camera(nullptr), switchSets(false) {
	light = LightActor(Vec4(26.0f, -5.0f, 10.0f, 1.0f), Vec4(0.0, 0.0f, 1.0f, 1.0f));
	light1 = LightActor(Vec4(6.0f, 5.0f, 5.0f, 1.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
	light2 = LightActor(Vec4(-20.0f, -5.0f, 10.0f, 1.0f), Vec4(1.0f, 0.0f, 1.0f, 1.0f));

	lightData.position.push_back(light.GetPosition());
	lightData.position.push_back(light1.GetPosition());
	lightData.position.push_back(light2.GetPosition());

	lightData.color.push_back(light.GetColor());
	lightData.color.push_back(light1.GetColor());
	lightData.color.push_back(light2.GetColor());

	camera = new Camera();
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
	if(camera) delete camera;

}

bool Scene0::OnCreate() {
	int width = 0, height = 0;
	float aspectRatio;
	switch (renderer->getRendererType()){
	case RendererType::VULKAN:
		
		SDL_GetWindowSize(dynamic_cast<VulkanRenderer*>(renderer)->GetWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		camera->Perspective(90.0f, aspectRatio, 0.5f, 20.0f);
		camera->LookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

		break;

	case RendererType::OPENGL:
		break;
	}

	return true;
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent) {
	if (sdlEvent.type == SDL_WINDOWEVENT) {
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			float aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
			camera->Perspective(45.0f, aspectRatio, 0.5f, 20.0f);
			break;
		
		}
	}
	if (sdlEvent.type == SDL_KEYDOWN) {
		switch (sdlEvent.key.keysym.sym) {
		case SDLK_t:
			switchSets = true;
			break;
		case SDLK_w:
			changeColor = true;
			length += 0.05f;
			break;
		case SDLK_s:
			changeColor = true;
			length -= 0.05f;
			break;
		case SDLK_r:
			changeColor = true;
			r = (r + 0.05f > 1.0f) ? 0.0f : r + 0.05f;
			break;
		case SDLK_g:
			changeColor = true;
			g = (g + 0.05f > 1.0f) ? 0.0f : g + 0.05f;
			break;
		case SDLK_b:
			changeColor = true;
			b = (b + 0.05f > 1.0f) ? 0.0f : b + 0.05f;
			break;

		}
	}
	
}
void Scene0::Update(const float deltaTime) {
	static float elapsedTime = 10;
	elapsedTime += deltaTime;
	mariosModelMatrix = MMath::rotate(elapsedTime * 90.0f, Vec3(0.0f, 1.0f, 0.0f)) * MMath::translate(Vec3(0.0f, 0.0f, -1.0f));

}

void Scene0::Render()  {

	switch (renderer->getRendererType()) {
	case RendererType::VULKAN:
		VulkanRenderer* vRenderer;
		vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		if (switchSets) { 
			vRenderer->switchDescriptors();
			switchSets = false;
		}
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), camera->GetViewMatrix());
		vRenderer->SetLightUBO(lightData.position, lightData.color);
		vRenderer->SetNormalUBO(length, r, g, b);
		vRenderer->SetPushConstants(mariosModelMatrix);
		

		vRenderer->Render();
		break;

	case RendererType::OPENGL:
		OpenGLRenderer* glRenderer;
		glRenderer = dynamic_cast<OpenGLRenderer*>(renderer);
		/// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		/// Draw your scene here
		
		
		glUseProgram(0);
		
		break;
	}
}


void Scene0::OnDestroy() {
	
}
