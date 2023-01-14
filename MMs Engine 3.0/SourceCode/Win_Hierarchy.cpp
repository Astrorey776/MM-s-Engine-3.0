#include "Application.h"
#include "Win_Hierarchy.h"
#include "imgui.h"
#include "GameObject.h" 
#define DROP_ID_HIERARCHY_NODES "hierarchy_node"

Win_Hierarchy::Win_Hierarchy(Application* app, bool start_enabled)
{

}

Win_Hierarchy::~Win_Hierarchy()
{

}

void Win_Hierarchy::Update_Ui()
{
	if (App->UI->Hierarchy_open == true) {

		ImGui::Begin("Hierarchy", &App->UI->Hierarchy_open);
		std::map<uint, Resource*>::iterator it = App->ResManager->resources.begin();
		for (int i = 0; i < App->scene_intro->scene->childs.size(); i++)
		{
			GameObjectHierarchyTree(App->scene_intro->scene->childs[i], i);
		}

		if (ImGui::BeginPopupContextWindow())
		{
			RightClick_Inspector_Menu();

			ImGui::EndPopup();
		}
		ImGui::End();
	}

}


void Win_Hierarchy::GameObjectHierarchyTree(GameObject* node, int id)
{
	ImGuiTreeNodeFlags node_flags = /*ImGuiTreeNodeFlags_DefaultOpen | */ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	int node_clicked = -1;

	const char* GameObjname = node->Name.c_str();

	if (App->scene_intro->selectedObj == node)
	{
		node_flags += ImGuiTreeNodeFlags_Selected;
	}
	if (node->childs.empty())
	{
		node_flags += ImGuiTreeNodeFlags_Leaf;
	}

	bool open = ImGui::TreeNodeEx(GameObjname, node_flags);

	if (ImGui::IsItemClicked())
	{
		if (!App->scene_intro->CreatingJoint)
		{
			App->UI->DeactivateGameObjects(App->scene_intro->scene);
			node->isSelected = true;
			App->scene_intro->selectedObj = node;
		}
		else
		{
			App->scene_intro->JointObj2 = node;
		}
		

	}
	App->UI->DropTrget_In_Inspector(node);
	if (ImGui::BeginDragDropTarget()) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);

		if (payload != nullptr)
		{
			if (payload->IsDataType(DROP_ID_HIERARCHY_NODES))
			{
				GameObject* obj = *(GameObject**)payload->Data;

				if (obj != nullptr) //The second part of this is bug that needs to be fixed
				{
					App->UI->ChangeParent(obj, node);
				}

			}
			//ImGui::ClearDragDrop();
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) {
		ImGui::SetDragDropPayload(DROP_ID_HIERARCHY_NODES, &node, sizeof(GameObject), ImGuiCond_Once);
		ImGui::Text(GameObjname);
		ImGui::EndDragDropSource();
	}

	if (open)
	{
		for (int i = 0; i < node->childs.size(); i++)
		{
			GameObjectHierarchyTree(node->childs[i], i);
		}

		ImGui::TreePop();
	}
}


void Win_Hierarchy::RightClick_Inspector_Menu()
{
	if (ImGui::MenuItem("Delete"))
	{
		if (App->scene_intro->selectedObj != nullptr) 
		{
			App->scene_intro->selectedObj->to_delete = true;
		}
	}

	if (ImGui::MenuItem("Create empty Child"))
	{
		App->UI->empty_GameObjects++;
		std::string obj = std::to_string(App->UI->empty_GameObjects);

		std::string name = "Empty_Child";
		name.append(obj);

		GameObject* empty_GameObject = new GameObject(name.c_str(), App->scene_intro->selectedObj);
		empty_GameObject->CreateComponent(ComponentType::TRANSFORM);
	}

	if (ImGui::MenuItem("Create empty GameObject"))
	{
		App->UI->empty_GameObjects++;
		std::string obj = std::to_string(App->UI->empty_GameObjects);

		std::string name = "Empty_GameObject";
		name.append(obj);

		GameObject* empty_GameObject = new GameObject(name.c_str(), App->scene_intro->scene);
		empty_GameObject->CreateComponent(ComponentType::TRANSFORM);
	}

}