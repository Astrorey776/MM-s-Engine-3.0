#include "Application.h"
#include "Win_AboutMenu.h"
#include "imgui.h"
#include "GameObject.h" 
#define DROP_ID_HIERARCHY_NODES "hierarchy_node"

Win_AboutMenu::Win_AboutMenu(Application* app, bool start_enabled)
{

}

Win_AboutMenu::~Win_AboutMenu()
{

}

void Win_AboutMenu::Update_Ui(bool show_windoww)
{
	if (show_windoww == true)
	{

		ImGui::Begin("Info", &show_windoww);
		ImGui::Text("MM's Engine 2.0"); //ImGui::SameLine(); ImGui::Text("Nidhogg engine ");
		ImGui::Text("");
		ImGui::TextWrapped("An engine made by a student of the 'Centre de la Imatge i de la Tecnologia Multimedia (CITM)'");
		ImGui::Text("");
		ImGui::Text("Made by:");
		ImGui::Text("Adrian Martin"); ImGui::SameLine();
		if (ImGui::Button("Adrian Martin"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/Astrorey776", NULL, NULL, SW_SHOWNORMAL);
		}

		ImGui::Separator();
		ImGui::Text("Libraries used");
		ImGui::BulletText("SDL 2.0");
		ImGui::BulletText("SDL Mixer 2.0");
		ImGui::BulletText("Cereal 1.2.2");
		ImGui::BulletText("Glew 2.0.0");
		ImGui::BulletText("ImGui 1.78");
		ImGui::BulletText("MathGeoLib 1.5");
		ImGui::BulletText("OpenGL 3.1");
		ImGui::BulletText("Assimp 3.1.1");
		ImGui::BulletText("Devil 1.7.8");
		ImGui::Separator();
		ImGui::Text("License:");
		ImGui::Text("MIT License");
		ImGui::TextWrapped("Copyright 2020 Pol de la Torre Solé & Albert Garcia Belerda ");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy");
		ImGui::TextWrapped("of this softwareand associated documentation files(the Software), to deal");
		ImGui::TextWrapped("in the Software without restriction, including without limitation the rights ");
		ImGui::TextWrapped("to use, copy, modify, merge, publish, distribute, sublicense, and /or sell ");
		ImGui::TextWrapped("copies of the Software, and to permit persons to whom the Software is");
		ImGui::TextWrapped("furnished to do so, subject to the following conditions :");
		ImGui::TextWrapped("");
		ImGui::TextWrapped("The above copyright noticeand this permission notice shall be included in all");
		ImGui::TextWrapped("copies or substantial portions of the Software.");
		ImGui::TextWrapped("THE SOFTWARE IS PROVIDED ¡'AS IS`, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
		ImGui::TextWrapped("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
		ImGui::TextWrapped("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
		ImGui::TextWrapped("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
		ImGui::TextWrapped("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
		ImGui::TextWrapped("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
		ImGui::TextWrapped("SOFTWARE.");
		ImGui::End();
	}
	if (show_windoww == false)
	{
		App->UI->show_About = false;

	}
}

