#include "Time.h"

bool Time::Engine_Active = false;
bool Time::Game_Paused = false;
Timer Time::Game_Timer;
float Time::delta_time_fisics = 0;
float Time::time_Since_Game_Start;

void Time::Start() 
{
	time_Since_Game_Start = 0;
	Engine_Active = true;
	Game_Timer.Start();
}

void Time::PreUpdate(float dt)
{

	if (Engine_Active == true) 
	{
		time_Since_Game_Start = Game_Timer.ReadSec();
		delta_time_fisics = (float)Game_Timer.Read() / 1000.0f;
		Game_Timer.Start();
	}
	if (Engine_Active == false)
	{
		delta_time_fisics = 0;
	}

	if (Game_Paused == true)
	{
		delta_time_fisics = 0;
	}
}

void Time::Update(float dt)
{

}

void Time::Play()
{
	Engine_Active = true; 
	Game_Paused = false;
}

void Time::Stop()
{
	delta_time_fisics = 0;
	Engine_Active = false;
	Game_Paused = false;
	Game_Timer.Stop();
}

void Time::Pause()
{
	delta_time_fisics = 0;
	Game_Paused = true;
	Game_Timer.Stop();
}

void Time::Resume()
{
	Game_Paused = false;
	Game_Timer.Continue();
}