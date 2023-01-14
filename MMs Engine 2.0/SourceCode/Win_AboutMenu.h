#ifndef __WIN_ABOUTMENU_H__
#define __WIN_ABOUTMENU_H__

#include "Globals.h"
#include "ModuleUI.h"

class Win_AboutMenu
{
public:
	Win_AboutMenu(Application* app, bool start_enabled = true);
	~Win_AboutMenu();
	static void Update_Ui(bool show_About);
};

#endif // !__WIN_ABOUTMENU_H__
