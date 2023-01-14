#include "Application.h"
#include "Win_CameraConfig.h"
#include "imgui.h"
#include "GameObject.h" 

Win_CameraConfig::Win_CameraConfig(Application* app, bool start_enabled)
{

}

Win_CameraConfig::~Win_CameraConfig()
{

}

void Win_CameraConfig::UpdateUi(ComponentCamera* camera)
{
	if (App->UI->Config_Camera_open) {
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (ImGui::TreeNodeEx("Camera Editor", node_flags))
		{
			ImGui::Separator();
			ImGui::Text("Camera configuration:");

			static bool Culling = false;
			ImGui::Checkbox("Culling", &Culling);
			if (Culling)
			{
				camera->cullingActive = true;
			}
			else {
				camera->cullingActive = false;
			}

			float p = camera->frustrum.farPlaneDistance;
			ImGui::SetNextItemWidth(200);
			ImGui::DragFloat("Far Plane", &p, 1, 0.01f, 999.0f);
			if (ImGui::IsItemActive())
			{
				camera->frustrum.farPlaneDistance = p;
			}

			float p1 = camera->frustrum.nearPlaneDistance;
			ImGui::SetNextItemWidth(200);
			ImGui::DragFloat("Near Plane", &p1, 0.5f, 0.01f, 999.0f);
			if (ImGui::IsItemActive())
			{
				camera->frustrum.nearPlaneDistance = p1;
			}

			float p2 = camera->GetFOV();
			ImGui::SetNextItemWidth(200);
			ImGui::DragFloat("Field of view", &p2, 0.5f, 50.0f, 120.0f);
			if (ImGui::IsItemActive())
			{
				camera->SetFOV(p2);
				//camera->frustrum.verticalFov = p2;
			}
			ImGui::Text("Horizontal FOV: %f", camera->GetHorizontalFov());
			ImGui::Text("Vertical FOV: %f", camera->GetFOV());
			ImGui::Text("Aspect ratio: %f", camera->aspectRatio);
			ImGui::TreePop();

		}

		
	}


}