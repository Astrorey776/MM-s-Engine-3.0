#ifndef __MOUSEPICKING_H__
#define __MOUSEPICKING_H__

#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"


class GameObject;
class MousePicking : public Module
{
public:

	MousePicking(Application* app, bool start_enabled = true);
	~MousePicking();

	bool Init();

	// Called before quitting
	bool CleanUp();
	void CastRay(LineSegment ray);

	void checkAABB(LineSegment ray, GameObject* parent); 
	bool checkTri(LineSegment ray, GameObject* parent);

	update_status PostUpdate(float dt);
private:
	void NewRay(LineSegment ray);

public:
	bool castRay;
	LineSegment newRay;
	float closestPoint;
};

#endif // __MOUSEPICKING_H__


