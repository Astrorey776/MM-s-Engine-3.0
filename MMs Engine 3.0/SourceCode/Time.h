#ifndef __TIME_H__
#define __TIME_H__

#include "Globals.h"
#include "Timer.h"

class Timer;

class Time {

public:
	static void Start();
	static void PreUpdate(float dt);
	static void Update(float dt);

	static void Play();
	static void Pause();
	static void Stop();
	static void Resume();

public:
	static float time_Since_Game_Start;
	static float time_Scale;
	static float delta_time_fisics;

	float time_Since_Engine_Start;
	static float delta_time_Engine;
	static bool Game_Paused;
	static Timer Game_Timer;
	static bool Engine_Active;

};
#endif // !__TIME_H__
