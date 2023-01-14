#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "imgui.h"
#include "Vec2.h"
class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();
	void Fullscreen_UI(bool active);
	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;
	//The surface contained by the window
	SDL_Surface* screen_surface;

	bool fullscren_Ui;
	bool setUIfullscreen;
	int i = 0;
	int e = 0;
	const char* glsl_version;
public:
	Vec2 windowSize;
};

#endif // __ModuleWindow_H__