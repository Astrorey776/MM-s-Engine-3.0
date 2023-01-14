#ifndef __WIN_INSPECTOR_H__
#define __WIN_INSPECTOR_H__

#include "Globals.h"
#include "ModuleUI.h"
class Collider;

class Win_Inspector
{

public:
	Win_Inspector(Application* app, bool start_enabled = true);
	~Win_Inspector();

	static void InspectorWin(); //gameobjects list
	static void GameObjectInspector(GameObject* obj);
	static void AddComponent(GameObject* obj);
	static void ColliderEditor(Collider*);
public:
	static float mass ;

};
#endif // !__WIN_INSPECTOR_H__
