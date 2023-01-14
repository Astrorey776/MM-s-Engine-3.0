#include "Application.h"
#include "Timer.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "PrimitiveManager.h"
#include "FBXloader.h"
#include "Time.h"
#include "ModulePhysics3D.h"
#include "ModulePlayer.h"

#include "FileSystem.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	UI = new ModuleUI(this);
	PrimManager = new PrimitiveManager(this);
	FBX = new FBXloader(this);
	file_system = new FileSystem(this);
	serializer = new Serializer(this);
	ResManager = new ResourceManager(this);
	MousePick = new MousePicking(this);
	Physics = new ModulePhysics3D(this);
	player = new ModulePlayer(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(file_system);
	AddModule(serializer);
	AddModule(Physics);
	AddModule(player);
	// Scenes
	AddModule(PrimManager);
	AddModule(ResManager);
	AddModule(FBX);
	AddModule(scene_intro);
	AddModule(UI);
	AddModule(MousePick);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	// release modules
	for (int i = list_modules.size() - 1; i >= 0; i--) {
	
		delete list_modules[i];
	}
	list_modules.clear();

}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules


	Module* it;

	for (int i = 0; i < list_modules.size() && ret == true; i++) {

		it = list_modules[i];
			ret = it->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (int i = 0; i < list_modules.size() && ret == true; i++) {

		it = list_modules[i];
		ret = it->Start();
	}

	last_sec_frame_time.Start();
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frame_count++;
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
	Time::PreUpdate(dt);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Time::Update(dt);
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		last_sec_frame_count = frame_count;
		frame_count = 0;
	}

	uint32_t last_frame_ms = ms_timer.Read();
	if (cap > 0 && last_frame_ms < cap)
	{
		SDL_Delay(cap - last_frame_ms);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	

	Module* it;

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {

			it = list_modules[i];
			ret = it->PreUpdate(dt);
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {

			it = list_modules[i];
			ret = it->Update(dt);
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {

			it = list_modules[i];
			ret = it->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	

	for (int i = list_modules.size() - 1; i >= 0 && ret == true; i--) {

		ret = list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}
int Application::GetFPS()
{
	return last_sec_frame_count;
}

void Application::Maxfps(int max_fps)
{
	fps = max_fps;
	if (fps == 0) fps = -1;
	cap = 1000 / fps;
}
