#include "Application.h"
#include "Win_Folder.h"
#include "imgui.h"

Win_Folder::Win_Folder(Application* app, bool start_enabled)
{

}

Win_Folder::~Win_Folder()
{

}

void Win_Folder::Update_Ui()
{
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Folders", &App->UI->Console_open))
	{
		{
			ImGui::BeginGroup();
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{



				if (ImGui::BeginTabItem("Assets"))
				{
					ImGui::PushID(-1);

					if (ImGui::ImageButton((ImTextureID)App->scene_intro->FolderGoBack, { 18, 18 }, ImVec2(0, 1), ImVec2(1, 0)))
					{
						if (App->UI->currentDirectory != "Assets")
						{
							size_t pos_separator = App->UI->currentDirectory.find_first_of("\\/") + 1;

							App->UI->currentDirectory = App->UI->currentDirectory.substr(0, pos_separator);
							App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
							App->UI->SortFilesinDir();

						}


					}

					ImGui::PopID();


					ImGui::Columns(4, NULL, false);


					for (int i = 0; i < App->UI->FilesInDir.size(); i++)
					{
						std::string filedir = App->UI->FilesInDir[i]->file.c_str();
						filedir += App->UI->FilesInDir[i]->extension.c_str();
						ImGui::PushID(i);

						if (App->UI->FilesInDir[i]->extension == "fbx" || App->UI->FilesInDir[i]->extension == "FBX")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}
						else if (App->UI->FilesInDir[i]->extension == "png")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}

						else if (App->UI->FilesInDir[i]->extension == "png")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}

						else if (App->UI->FilesInDir[i]->extension == "tga")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}

						else if (App->UI->FilesInDir[i]->extension == "meta")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}
						else if (App->UI->FilesInDir[i]->extension == "mesh")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}
						else if (App->UI->FilesInDir[i]->extension == "dds")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}
						else if (App->UI->FilesInDir[i]->extension == "json")
						{
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}							
						}

						else if (App->UI->FilesInDir[i]->extension == "model")
						{							
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}
						}
						else {

							if (ImGui::ImageButton((ImTextureID)App->scene_intro->FolderIco, { 1, 1 }, ImVec2(0, 1), ImVec2(1, 0)))
							{
								LOG("%s", filedir.c_str());
								if (App->UI->FilesInDir[i]->extension == "")
								{
									App->UI->currentDirectory = App->UI->FilesInDir[i]->fullpath.c_str();
									App->file_system->checkDirectoryFiles(App->UI->currentDirectory.c_str(), &App->UI->FilesInDir);
									App->UI->SortFilesinDir();

								}
							}
						}

						const bool is_hovered = ImGui::IsItemHovered(); // Hovered

						if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							App->UI->clickedAsset = App->UI->FilesInDir[i]->fullpath;
						}


						ImGui::PopID();

						ImGui::Text("%s", filedir.c_str());
						ImGui::Dummy(ImVec2(0.0f, 20.0f));

						ImGui::NextColumn();

					}
					ImGui::Columns(1);

					if (ImGui::BeginPopupContextWindow())
					{
						App->UI->RightClick_Assets_Menu(App->UI->clickedAsset.c_str());
						ImGui::EndPopup();
					}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Console"))
				{
					for (int i = 0; i < App->UI->consoleOutput.size(); i++)
					{
						const char* text = App->UI->consoleOutput[i].c_str();
						ImGui::Text(text);
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::EndGroup();
		}
	}
	ImGui::End();

}
