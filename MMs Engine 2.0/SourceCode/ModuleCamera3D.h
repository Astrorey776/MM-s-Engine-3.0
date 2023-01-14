#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;
class ComponentCamera;
class ComponentTransform;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float2 ScreenToWorld(float2 pos);
	float* GetViewMatrix();
	float* GetFustrumMatrix();
	float* GetFustrumProjMatrix();
private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;
	Color background;
	GameObject* cameraOBJ;
	ComponentCamera* cameraComp;
	ComponentTransform* cameraTrans;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	float4x4 cameraMat, cameraMatInverse;
	bool frustrumFollow;
};