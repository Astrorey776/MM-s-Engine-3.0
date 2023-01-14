#ifndef __WIN_FOLDER_H__
#define __WIN_FOLDER_H__

#include "Globals.h"
#include "ModuleUI.h"

class Win_Folder
{
public:
	Win_Folder(Application* app, bool start_enabled = true);
	~Win_Folder();
	static void Update_Ui();
};

#endif // !__WIN_FOLDER_H__

