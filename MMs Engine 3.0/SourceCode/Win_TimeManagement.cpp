#include "Application.h"
#include "Win_TimeManagement.h"
#include "imgui.h"
#include "GameObject.h" 
#include "Time.h"

Win_TimeManagement::Win_TimeManagement(Application * app, bool start_enabled)
{

}

Win_TimeManagement::~Win_TimeManagement()
{

}

void Win_TimeManagement::Update_Ui()
{

	if (App->UI->Inspector_open == true)
	{
		ImGui::Begin("Time", (bool*)false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);
	


		if (Time::Engine_Active == false) {
			
			if (ImGui::Button("PLAY"))
			{
				Time::Start();
				App->serializer->CreateNewScene();
				App->scene_intro->SaveScene();
				App->serializer->SaveScene("PlayScene.json");
			} 
		}
		else {

			if (Time::Game_Paused == false)
			{
				if (ImGui::Button("PAUSE"))
				{
					Time::Pause();

				} 
			}

			if (Time::Game_Paused == true)
			{
				if (ImGui::Button("CONTINUE"))
				{
					Time::Resume();

				}
			}

			if (ImGui::Button("STOP"))
			{
				App->scene_intro->DeleteSceneObjects(App->scene_intro->scene);
				App->serializer->LoadScene("Assets/PlayScene.json");
				Time::Stop();
			}

		}

		ImGui::End();
	}
}