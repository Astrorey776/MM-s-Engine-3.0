#ifndef __WIN_HIERARCHY_H__
#define __WIN_HIERARCHY_H__

#include "Globals.h"
#include "ModuleUI.h"

class Win_Hierarchy
{
public:
	Win_Hierarchy(Application* app, bool start_enabled = true);
	~Win_Hierarchy();
	static void Update_Ui();
	static void GameObjectHierarchyTree(GameObject* node, int id);
	static void RightClick_Inspector_Menu();
};

#endif // !__WIN_HIERARCHY_H__
