#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Primitive;
class Cube;
class Pyramid;
class PrimSphere;
class PrimCylinder;
class PrimLine;
class PrimNormals;
class PrimAABB;

class PrimitiveManager : public Module
{
public:
	PrimitiveManager(Application* app, bool start_enabled = true);
	~PrimitiveManager();

	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();
	Cube* CreateCube(vec3 size, vec3 pos = (0.f,0.f,0.f));
	Pyramid* CreatePyramid(vec3 size, vec3 pos = (0.f, 0.f, 0.f));
	PrimSphere* CreateSphere(float radius, unsigned int rings, unsigned int sectors, vec3 pos = (0.f, 0.f, 0.f));
	PrimCylinder* CreateCylinder(float radius, float height, int sides = 30, vec3 pos = (0.f, 0.f, 0.f));
	PrimLine* CreateLine(float3 origin, float3 dest);
	PrimNormals* CreateNormalVects(float* normal_array, int size);
	PrimAABB* CreateAABB(AABB* bbox);

	void CreatePlane(vec4 coords, bool axis = true);

	void CreateFBXCube();
	void CreateFBXPyramid();
	void CreateFBXSphere();
	void CreateFBXCylinder();

	void MenuOptions();
	void ShowWireframe();
	void HideWireframe();
private:
	
public:
	vector<Primitive*> prim_list;
	bool depth_bool;
	bool cullface_bool;
	bool texture2D_bool;
	bool LIGHTING_bool;
	bool Color_bool;
};
