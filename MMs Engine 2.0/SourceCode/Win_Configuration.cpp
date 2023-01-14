#include "Application.h"
#include "Win_Configuration.h"
#include "imgui.h"
#include "GameObject.h" 
#include "glew/include/glew.h"

Win_Configuration::Win_Configuration(Application* app, bool start_enabled)
{

}

Win_Configuration::~Win_Configuration()
{

}

void Win_Configuration::UpdateUi(bool show_config)
{
	if (show_config == true)
	{
		ImGui::Begin("Configuration", &show_config);

		ImGui::MenuItem("Wellcome to the Configuration menu", NULL, false, false);

		if (ImGui::CollapsingHeader("Application"))
		{
			static char buf[32] = "";
			ImGui::InputText("App name", buf, IM_ARRAYSIZE(buf));
			SDL_SetWindowTitle(App->window->window, buf);

			static char buf2[32] = "";
			ImGui::InputText("Organitzation", buf2, IM_ARRAYSIZE(buf));
			PlotGraph();
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			static bool Wireframe_visible = false;
			ImGui::Checkbox("Wirframe visible", &Wireframe_visible); ImGui::SameLine();
			if (Wireframe_visible)
			{
				if (App->UI->Wireframe_bool)
				{
					App->UI->Wireframe_bool = false;
					App->PrimManager->ShowWireframe();
				}

			}
			else {
				if (!App->UI->Wireframe_bool)
				{
					App->PrimManager->HideWireframe();
				}
				App->UI->Wireframe_bool = true;

			}

			static bool Depth = false;
			ImGui::Checkbox("Deph", &Depth);
			if (Depth) {
				App->PrimManager->depth_bool = true;
			}
			else {
				App->PrimManager->depth_bool = false;
			}

			static bool cullface = false;
			ImGui::Checkbox("CullFace", &cullface); ImGui::SameLine();
			if (cullface) {
				App->PrimManager->cullface_bool = true;
			}
			else {
				App->PrimManager->cullface_bool = false;
			}

			static bool texture2D = true;
			ImGui::Checkbox("Texture 2D", &texture2D);
			if (texture2D) {
				App->PrimManager->texture2D_bool = true;
			}
			else {
				App->PrimManager->texture2D_bool = false;
			}

			static bool Lighting = false;
			ImGui::Checkbox("No Lighting", &Lighting); ImGui::SameLine();
			if (Lighting) {
				App->PrimManager->LIGHTING_bool = true;
			}
			else {
				App->PrimManager->LIGHTING_bool = false;
			}

			static bool Color = false;
			ImGui::Checkbox("Disable Colors", &Color);
			if (Color) {
				App->PrimManager->Color_bool = true;
			}
			else {
				App->PrimManager->Color_bool = false;
			}


			static float f1 = 1.0f;
			ImGui::SliderFloat("Brightness", &f1, 0.0f, 1.0f, "%.3f");
			if (ImGui::IsItemActive())
			{
				SDL_SetWindowBrightness(App->window->window, f1);
			}

			static int i1 = App->window->screen_surface->w;
			static int i2 = App->window->screen_surface->h;
			ImGui::SliderInt("width", &i1, 640, 1920);
			if (ImGui::IsItemActive())
			{
				SDL_SetWindowSize(App->window->window, i1, i2);
				//App->renderer3D->OnResize(i1, i2); if we don not coment this will change the width and heigh from the
			}
			ImGui::SliderInt("height", &i2, 480, 1080);
			if (ImGui::IsItemActive())
			{
				SDL_SetWindowSize(App->window->window, i1, i2);
				//App->renderer3D->OnResize(i1, i2);
			}

			ImGui::Text("Refresh rate: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%d ", App->GetFPS());

			ImGui::Checkbox("Fullscreen", &App->UI->active2); ImGui::SameLine();
			App->window->Fullscreen_UI(App->UI->active2);



			ImGui::Checkbox("Resizable", &App->UI->resizable_bool);
			static int j = 0;
			if (App->UI->resizable_bool == false && j == 0)
			{
				j = 1;
				SDL_SetWindowResizable(App->window->window, SDL_FALSE);
			}
			else if (App->UI->resizable_bool == true && j == 1)
			{
				j = 0;
				SDL_SetWindowResizable(App->window->window, SDL_TRUE);
			}

			static int j2 = 0;
			ImGui::Checkbox("Borderless", &App->UI->border_bool); ImGui::SameLine();
			if (App->UI->border_bool == false && j2 == 0)
			{
				j2 = 1;
				SDL_SetWindowBordered(App->window->window, SDL_TRUE);
			}
			else if (App->UI->border_bool == true && j2 == 1)
			{
				j2 = 0;
				SDL_SetWindowBordered(App->window->window, SDL_FALSE);
			}

			static bool bool_fullscreeen_Desktop = false;
			ImGui::Checkbox("Full Desktop", &bool_fullscreeen_Desktop);

			if (bool_fullscreeen_Desktop == true && App->UI->Screen_i == 0)
			{
				App->UI->Screen_e = 0;
				App->UI->Screen_i++;
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			if (bool_fullscreeen_Desktop == false && App->UI->Screen_e == 0)
			{
				SDL_SetWindowFullscreen(App->window->window, 0);
				App->UI->Screen_i = 0;
				App->UI->Screen_e++;
			}
		}

		if (ImGui::CollapsingHeader("File System"))
		{

		}

		if (ImGui::CollapsingHeader("Input"))
		{
			int mousex = 0; int mousey = 0;
			SDL_GetMouseState(&mousex, &mousey);
			ImGui::Text("Mouse Position:"); ImGui::SameLine();  ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%dx %dy", mousex, mousey);

			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("Mouse Motion: ");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "% g, % g", io.MouseDelta.x, io.MouseDelta.y);

			ImGui::Text("Mouse wheel:"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%.0f", io.MouseWheel);
			ImGui::Separator();

			ImGui::Text("Mouse down:");
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
				if (io.MouseDownDuration[i] >= 0.0f)
				{
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "b%d (%.02f secs)", i, io.MouseDownDuration[i]);
				}

			ImGui::Text("Keys down:");
			for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
				if (io.KeysDownDuration[i] >= 0.0f)
				{
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]);
				}
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			SDL_version compiled;
			SDL_VERSION(&compiled);
			static bool active = false;
			ImGui::Checkbox("Active", &active);
			ImGui::Text("SDL Version :"); ImGui::SameLine();  ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);
			ImGui::Separator(); ImGui::Spacing();
			ImGui::Text("Number of logical CPU cores:"); ImGui::SameLine();  ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%d cores (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

			ImGui::Text("System Ram:"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%dGB", (SDL_GetSystemRAM() / 1000)); ImGui::Spacing();

			ImGui::Text("Caps:"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1.f));
			if (SDL_HasRDTSC())ImGui::Text("RDTSC,"); ImGui::SameLine();
			if (SDL_HasMMX())ImGui::Text("MMX,"); ImGui::SameLine();
			if (SDL_HasMMX())ImGui::Text("MMX,"); ImGui::SameLine();
			if (SDL_HasAltiVec())ImGui::Text("AltiVec,"); ImGui::SameLine();
			if (SDL_Has3DNow())ImGui::Text("3DNow,"); ImGui::SameLine();
			if (SDL_HasSSE())ImGui::Text("SSE,"); ImGui::SameLine();
			if (SDL_HasSSE2())ImGui::Text("SSE2,"); ImGui::SameLine();
			if (SDL_HasSSE3())ImGui::Text("SSE3,"); ImGui::SameLine();
			if (SDL_HasSSE41())ImGui::Text("SSE41,"); ImGui::SameLine();
			if (SDL_HasSSE42())ImGui::Text("SSE42,"); ImGui::SameLine();
			if (SDL_HasAVX())ImGui::Text("AVX,"); ImGui::SameLine();
			if (SDL_HasAVX2())ImGui::Text("AVX2,"); ImGui::SameLine();
			if (SDL_HasAVX512F())ImGui::Text("AVX512F,"); ImGui::SameLine();
			if (SDL_HasARMSIMD())ImGui::Text("ARMSIMD,"); ImGui::SameLine();
			if (SDL_HasNEON())ImGui::Text("NEON, ");
			ImGui::PopStyleColor();

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Brand: ");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%s", glGetString(GL_VENDOR));// Returns the vendor
			ImGui::Text("Graphic Card: ");  ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1.f), "%s", glGetString(GL_RENDERER));// Returns a hint to the model
		}

		ImGui::End();
	}

	if (show_config == false)
	{
		App->UI->show_Configuration = false;

	}
}


void Win_Configuration::PlotGraph()
{
	if (ImGui::SliderInt("Maximum FPS", &App->UI->max_fps, -1, 200, NULL))
	{
		App->Maxfps(App->UI->max_fps);
	}
	bool active = true;
	int fps = App->GetFPS();
	if (App->UI->fpsecond.size() > 120)
	{
		for (int i = 1; i < App->UI->fpsecond.size(); i++)
		{
			App->UI->fpsecond[i - 1] = App->UI->fpsecond[i];      //with this we change the frist for the 2nd to update all
		}
		App->UI->fpsecond[App->UI->fpsecond.size() - 1] = fps;
	}
	else
	{
		App->UI->fpsecond.push_back(fps);
	}
	char text[20];
	sprintf_s(text, 20, "Frames: %d", fps);
	ImGui::Text(text);
	ImGui::PlotHistogram("Framerate", &App->UI->fpsecond[0], App->UI->fpsecond.size(), 0, text, 0.0f, 100.0f, ImVec2(450, 100));

}