#ifndef __WIN_CONFIGURATION_H__
#define __WIN_CONFIGURATION_H__

#include "Globals.h"
#include "ModuleUI.h"

class Win_Configuration
{
public:
	Win_Configuration(Application* app, bool start_enabled = true);
	~Win_Configuration();
	static void UpdateUi(bool show_config);
	static void PlotGraph();
};


#endif // !__WIN_CONFIGURATION_H__