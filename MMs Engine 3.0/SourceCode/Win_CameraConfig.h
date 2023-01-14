#ifndef __WIN_CAMERACONFIG_H__
#define __WIN_CAMERACONFIG_H__

#include "Globals.h"
#include "ModuleUI.h"

class Win_CameraConfig
{
public:
	Win_CameraConfig(Application* app, bool start_enabled = true);
	~Win_CameraConfig();
	static void UpdateUi(ComponentCamera* camera);

};


#endif // !__WINDOW_CAMERACONFIG_H__

