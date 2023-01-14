#include "Globals.h"
#include "Application.h"
#include "PrimitiveManager.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "glew/include/glew.h"

#include "MathGeoLib/include/MathGeoLib.h"



PrimitiveManager::PrimitiveManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

PrimitiveManager::~PrimitiveManager()
{}

// Load assets
bool PrimitiveManager::Start()
{
	bool ret = true;
	depth_bool = false;
	cullface_bool = false;	
	texture2D_bool = true;
	LIGHTING_bool = false;
	Color_bool = false;
	return ret;
}

// Load assets
bool PrimitiveManager::CleanUp()
{
	LOG("Unloading Primitive Manager");
	for (int i = 0; i < prim_list.size(); i++) {

		delete prim_list[i];

	}
	return true;
}

// Update: draw background
update_status PrimitiveManager::PostUpdate(float dt)
{	
	MenuOptions();

	for (int i = 0; i < prim_list.size(); i++) {
		prim_list[i]->Update();
		prim_list[i]->Render();
		
	}
	
	for (int i = 0; i < prim_list.size(); i++) {
		if (prim_list[i]->to_delete)
		{
			if (prim_list[i] != NULL)        
			{                      
				delete prim_list[i];
				prim_list[i] = NULL;
			}                      
			prim_list.erase(prim_list.begin() + i);
			i--;
		}
	}

	return UPDATE_CONTINUE;
}
void PrimitiveManager::ShowWireframe() 
{
	for (int i = 0; i < prim_list.size(); i++)
	{

		prim_list[i]->wire = !prim_list[i]->wire;
	}
}
void PrimitiveManager::HideWireframe() 
{
	for (int i = 0; i < prim_list.size(); i++)
	{

		prim_list[i]->wire = false;
	}
}

void PrimitiveManager::CreateFBXCube() {
	std::string file_path = "Assets/Primitives/Cube.FBX";
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->file_system->Load(file_path.c_str(), &buffer);
	App->FBX->LoadFBX(buffer, fileSize);
}
void PrimitiveManager::CreateFBXPyramid() {
	std::string file_path = "Assets/Primitives/Pyramid.FBX";
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->file_system->Load(file_path.c_str(), &buffer);
	App->FBX->LoadFBX(buffer, fileSize);
}
void PrimitiveManager::CreateFBXSphere() {
	std::string file_path = "Assets/Primitives/Sphere.FBX";
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->file_system->Load(file_path.c_str(), &buffer);
	App->FBX->LoadFBX(buffer, fileSize);
}
void PrimitiveManager::CreateFBXCylinder() {
	std::string file_path = "Assets/Primitives/Cylinder.FBX";
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->file_system->Load(file_path.c_str(), &buffer);
	App->FBX->LoadFBX(buffer, fileSize);
}

Cube* PrimitiveManager::CreateCube(vec3 size, vec3 pos)
{
	Cube* cube = new Cube(size.x, size.y, size.z);
	cube->SetPos(pos.x, pos.y, pos.z);
	cube->body.SetBody(cube, 1);

	prim_list.push_back((Primitive*)cube);
	return cube;
}

Pyramid* PrimitiveManager::CreatePyramid(vec3 size, vec3 pos)
{
	Pyramid* pyramid = new Pyramid(size.x, size.y, size.z);
	pyramid->SetPos(pos.x, pos.y, pos.z);
	prim_list.push_back((Primitive*)pyramid);
	return pyramid;
}


PrimSphere* PrimitiveManager::CreateSphere(float radius, unsigned int rings, unsigned int sectors, vec3 pos)
{
	PrimSphere* sphere = new PrimSphere(radius, rings, sectors);
	sphere->SetPos(pos.x, pos.y, pos.z);
	sphere->body.SetBody(sphere, 1);

	prim_list.push_back((Primitive*)sphere);
	return sphere;
}

PrimCylinder* PrimitiveManager::CreateCylinder(float radius, float height, int sides, vec3 pos)
{
	PrimCylinder* cylinder = new PrimCylinder(radius, height, sides);
	cylinder->SetPos(pos.x, pos.y, pos.z);
	prim_list.push_back((Primitive*)cylinder);
	return cylinder;
}

PrimLine* PrimitiveManager::CreateLine(float3 origin, float3 dest)
{
	PrimLine* line = new PrimLine(origin.x, origin.y, origin.z, dest.x, dest.y, dest.z);
	prim_list.push_back((Primitive*)line);
	return line;
}
void PrimitiveManager::CreatePlane(vec4 coords, bool axis)
{
	PrimPlane* plane = new PrimPlane(coords.x, coords.y, coords.z, coords.w);
	plane->axis = true;
	prim_list.push_back((Primitive*)plane);
}

PrimNormals* PrimitiveManager::CreateNormalVects(float* normal_array, int size)
{
	PrimNormals* normals = new PrimNormals(normal_array, size);
	prim_list.push_back((PrimNormals*)normals);
	return normals;
}

PrimAABB* PrimitiveManager::CreateAABB(AABB* bbox)
{
	PrimAABB* primAABB = new PrimAABB(bbox);
	prim_list.push_back((PrimAABB*)primAABB);
	return primAABB;
}

void PrimitiveManager::MenuOptions() 
{
	if (depth_bool)
	{
		glDisable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
	}

	if (cullface_bool)
	{
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	if (texture2D_bool)
	{
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
	}

	if (LIGHTING_bool) {
		glDisable(GL_LIGHTING);
	}
	else
	{
		glEnable(GL_LIGHTING);
	}

	if (Color_bool)
	{
		glDisable(GL_COLOR_MATERIAL);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
	}



}