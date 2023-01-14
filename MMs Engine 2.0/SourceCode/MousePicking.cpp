#include "Application.h"
#include "MousePicking.h"
#include "GameObject.h"
#include "ModuleUI.h"

#include "MathGeoLib/include/MathGeoLib.h"

MousePicking::MousePicking(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	castRay = false;

}


MousePicking::~MousePicking()
{
	
}

bool MousePicking::Init()
{
	LOG("Initializing mouse picking");
	return true;
}

bool MousePicking::CleanUp()
{
	return false;
}
update_status MousePicking::PostUpdate(float dt)
{
	if (castRay && App->UI->using_gizmo == false)
	{
		castRay = false;
		NewRay(newRay);
	}
	else if (castRay)
	{
		castRay = false;
	}

	return UPDATE_CONTINUE;
}

void MousePicking::CastRay(LineSegment ray)
{
	newRay = ray;
	castRay = true;
}

void MousePicking::NewRay(LineSegment ray)
{
	if (App->scene_intro->selectedObj != nullptr)
		App->scene_intro->selectedObj->isSelected = false;
	if(!App->scene_intro->CreatingJoint)
	App->scene_intro->selectedObj = nullptr;

	closestPoint = inf;
	checkAABB(ray, App->scene_intro->scene);
}

void MousePicking::checkAABB(LineSegment ray, GameObject* parent)
{
	float close = 0, null = 0;
	bool hit = ray.Intersects(parent->aabb, close, null); // ray vs. AABB

	if (hit == true)
	{
		bool hit_tri = checkTri(ray, parent);
		if (close < closestPoint && hit_tri)
		{
			closestPoint = close;
			if (App->scene_intro->selectedObj != nullptr && !App->scene_intro->CreatingJoint)
			App->scene_intro->selectedObj->isSelected = false;
			if (!App->scene_intro->CreatingJoint)
			{
				App->scene_intro->selectedObj = parent;
				parent->isSelected = true;
			}
	
			else
			{
				App->scene_intro->JointObj2 = parent;
			}
		}

	}
	for (int i = 0; i < parent->childs.size(); i++)
	{
		checkAABB(ray, parent->childs[i]);
	}

}

bool MousePicking::checkTri(LineSegment ray, GameObject* parent)
{
	bool hit = false;
	float distance = 0, null = 0;
	float3 intersecPoint;
	Triangle tri;
	
	ComponentMesh* myMesh = (ComponentMesh*)parent->GetComponent(ComponentType::MESH);
	ComponentTransform* transform = (ComponentTransform*)parent->GetComponent(ComponentType::TRANSFORM);
	LineSegment ray_local_space = ray;
	ray_local_space.Transform(transform->global_transform.Inverted());

	if (myMesh != nullptr)
	{
		for (int i = 0; i < myMesh->num_index && hit == false; i++)
		{
			float total = myMesh->num_vertex * 3;
			int it = myMesh->index[i];

			tri.a.Set(myMesh->vertex[it * 3], myMesh->vertex[it * 3 + 1], myMesh->vertex[it * 3 + 2]);
			i++;
			it = myMesh->index[i];
			tri.b.Set(myMesh->vertex[it * 3], myMesh->vertex[it * 3 + 1], myMesh->vertex[it * 3 + 2]);
			i++;
			it = myMesh->index[i];
			tri.c.Set(myMesh->vertex[it * 3], myMesh->vertex[it * 3 + 1], myMesh->vertex[it * 3 + 2]);

			/*App->PrimManager->CreateLine(tri.a, tri.b);
			App->PrimManager->CreateLine(tri.b, tri.c);
			App->PrimManager->CreateLine(tri.c, tri.a);*/

			hit = ray_local_space.Intersects(tri, &distance, &intersecPoint); // ray vs. triangle
			if (hit)
			{
				LOG("intersection with a tri");
			}
		}
	}

	return hit;
}