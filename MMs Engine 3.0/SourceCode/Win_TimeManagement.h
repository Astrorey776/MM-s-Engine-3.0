#ifndef __WIN_TIMEMANAGEMENT_H__
#define __WIN_TIMEMANAGEMENT_H__

#include "Globals.h"
#include "ModuleUI.h"

class Win_TimeManagement
{
public:
	Win_TimeManagement(Application* app, bool start_enabled = true);
	~Win_TimeManagement();
	static void Update_Ui();
};

#endif //!__WIN_TIMEMANAGEMENT_H__
