#include "Globals.h"
#include "Application.h"
#include "ModuleUi.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "ModuleSceneIntro.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "Win_CameraConfig.h"
#include "Win_Hierarchy.h"
#include "Win_Inspector.h"
#include "Win_Configuration.h"
#include "Win_AboutMenu.h"
#include "Win_TimeManagement.h"
#include "Win_Folder.h"
#include "FileSystem.h"
#include "glew/include/glew.h"        
#define DROP_ID_HIERARCHY_NODES "hierarchy_node"

ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	io = new ImGuiIO;
}

ModuleUI::~ModuleUI()
{}

bool ModuleUI::Init()
{
	App->renderer3D->context = SDL_GL_CreateContext(App->window->window);
	SDL_GL_MakeCurrent(App->window->window, App->renderer3D->context);

	bool err = glewInit();

	if (err)
	{
		LOG("Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// Setup Dear ImGui style
	ImGui::StyleColorsLight();
	 // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init(App->window->glsl_version);

// Load assets

	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	c1 = 0;
	show_demo_window = false;
	show_console = false;
	show_About = false;
	show_Configuration = false;
	active2 = false;
	show_another_window = true;
	resizable_bool = false;
	border_bool = false;
	Wireframe_bool = false;
	Hierarchy_open = true;
	Assetstree_open = true;
	importWindow = false;
	Config_Camera_open = true;
	FisicsConfig_open = true;
	Inspector_open = true;
	Console_open = true;
	direction_camera = { 0,0,0 };
	cameras = 0;
	empty_GameObjects = 0;
	width = 960;
	height = 540;
	LocalGuizmo = false;
	WorldGuizmo = true;
	guizmo_mode = ImGuizmo::MODE::WORLD;
	Screen_i = 0;
	Screen_e = 1;
	int max_fps = 61;
	App->Maxfps(max_fps);
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	selectedAsset = nullptr;
	currentDirectory = "Assets";
	App->file_system->checkDirectoryFiles(currentDirectory.c_str(), &FilesInDir);
	SortFilesinDir();
	currentFolderDirectory= "Assets";
	FoldersInDir = new FolderNode(currentFolderDirectory,nullptr);
	App->file_system->checkDirectoryFolders(currentDirectory.c_str(),FoldersInDir);
	selectedFolder = FoldersInDir;

	SortFilesinDir();
	return ret;
}

// Load assets
bool ModuleUI::CleanUp()
{
	LOG("Unloading UI");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

// Update: draw background
update_status ModuleUI::Update(float dt)
{
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	
	ShowAppinDockSpace(open_docking);
	
	
	ImGui::Begin("Game");
	winSize = ImGui::GetWindowSize();   //this will pick the current window size
	winPos = ImGui::GetWindowPos();


	if (winSize.x != windowSize.x || winSize.y != windowSize.y)
	{
		Change_Window_size(Vec2(winSize.x, winSize.y));
	}
	ImGui::SetCursorPos(ImVec2(image_offset.x, image_offset.y));

	img_corner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

	int x = 0;
	int y = 0;
	SDL_GetWindowPosition(App->window->window, &x, &y);
	img_corner -= Vec2(x, y);
	
	imgcorner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y) + Vec2(0, image_size.y);
	imgcorner.y =App->window->windowSize.y - imgcorner.y; //ImGui 0y is on top so we need to convert 0y on botton

	ImGui::Image((ImTextureID)App->renderer3D->texColorBuffer, ImVec2(image_size.x, image_size.y), ImVec2(0, 1), ImVec2(1, 0));
	ControlsGuizmo();
	GuizmoUI();
	ImGui::End();

	ImGui::BeginMainMenuBar(); //this creates the top bar
	 

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save Scene")) 
		{
			App->serializer->CreateNewScene();
			App->scene_intro->SaveScene();
			App->serializer->SaveScene("Scene.json");
		}
		if (ImGui::MenuItem("Load Scene"))
		{
			App->scene_intro->DeleteSceneObjects(App->scene_intro->scene);
			App->serializer->LoadScene("Assets/Scene.json");
		}
		ImGui::Separator();
		if (ImGui::Button("Exit"))
		{
			return UPDATE_STOP;
		}
		ImGui::EndMenu();
	}
	
	if (ImGui::BeginMenu("More"))
	{

		if (ImGui::MenuItem("Documentation"))
			ShellExecuteA(NULL, "open", "https://github.com/Astrorey776/MM-s-Engine-2.0", NULL, NULL, SW_SHOWNORMAL);

		if (ImGui::MenuItem("About"))
			show_About = true;


		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{

		if (ImGui::MenuItem("Configuration"))
			show_Configuration = true;

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("GameObjects"))
	{
		if (ImGui::BeginMenu("Create...")) {

			if (ImGui::MenuItem("Cube")) {
				App->PrimManager->CreateFBXCube();
			}
			if (ImGui::MenuItem("Pyramid")){
				App->PrimManager->CreateFBXPyramid();
			}
			if (ImGui::MenuItem("Sphere")) {
				App->PrimManager->CreateFBXSphere();
			}
			if (ImGui::MenuItem("Cylinder")) {
				App->PrimManager->CreateFBXCylinder();
			}
			
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Empty GameObject"))
		{
			empty_GameObjects++;
			std::string obj = std::to_string(empty_GameObjects);

			std::string name = "Empty_GameObject";
			name.append(obj);

			GameObject* empty_GameObject = new GameObject(name.c_str(), App->scene_intro->scene);
			empty_GameObject->CreateComponent(ComponentType::TRANSFORM);
		}
		ImGui::EndMenu();
	}
	
	ImGui::EndMainMenuBar();
	
	if (show_demo_window == true)
		ImGui::ShowDemoWindow(&show_demo_window);
	
	if (Console_open == true)Win_Folder::Update_Ui();
	Win_AboutMenu::Update_Ui(show_About);
	Win_Configuration::UpdateUi(show_Configuration);
	Win_Hierarchy::Update_Ui();
	AssetsTree();
	ImportWindow();
	Win_CameraConfig::UpdateUi(App->camera->cameraComp);
	ChangePhysicsWindow();

	Win_Inspector::InspectorWin();
	Win_TimeManagement::Update_Ui();
	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModuleUI::PostUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

	return UPDATE_CONTINUE;
}

void ModuleUI::StoreLog(const char* message)
{
	string str;
	str = message;
	
	consoleOutput.push_back(str);
}

void ModuleUI::ShowAppinDockSpace(bool* p_open)
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
ImGui::PopStyleVar();

if (opt_fullscreen)
ImGui::PopStyleVar(2);

// DockSpace
ImGuiIO& io = ImGui::GetIO();
if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
{
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}
else {
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

}

ImGui::End();

}

AssetNode* ModuleUI::createAssetNode(Resource* resource)
{
	AssetNode* node = new AssetNode(resource);
	assets.push_back(node);
	return node;
}


void ModuleUI::AssetsTree()
{
	if (Assetstree_open == true) {

		

		for (int i = 0; i < assets.size(); i++)
		{

			if (assets[i]->to_delete)
			{
				assets.erase(App->UI->assets.begin() + i);
				i--;
			}
			else
			AssetsHierarchyTree(assets[i]);
		}

		
	}
}

ImportOptions* ModuleUI::CreateImportObject(const char* importpath, importType type)
{
	ImportOptions* newimport;
	switch (type)
	{
	case importType::UNKNOWN:
		break;
	case importType::MODEL:
		newimport = new ModelOptions(importpath);
		break;
	case importType::TEXTURE:
		newimport = new TextureOptions(importpath);
		break;
	}
	importsvec.push_back(newimport);
	return newimport;
}
void ModuleUI::ImportWindow()
{
	if (importsvec.size() > 0)
		importWindow = true;
	else
		importWindow = false;

	if (importWindow)
	{
		ImGui::Begin("ImportWindow", &importWindow);
		int id = 0;
		ImGui::Text(importsvec[id]->path.c_str());
			switch (importsvec[id]->type)
			{
			case importType::MODEL:
			{
				ModelOptions* modeloptions = (ModelOptions*)importsvec[id];
				static bool axis = false;
				static float f0 = modeloptions->GlobalScale;
				ImGui::InputFloat("GlobalScale", &f0, 0.01f, 1.0f, "%.3f");
				modeloptions->GlobalScale = f0;


				ImGui::Checkbox("Axis", &axis);
				if (axis)
				{
					modeloptions->axis = true;
				}
				static bool cameras = false;

				ImGui::Checkbox("Ignore cams and lights", &cameras);
				if (cameras)
				{
					modeloptions->ignoreCameras = true;
				}
				break;
			}
				
			case importType::TEXTURE:
			{
				TextureOptions* texoptions = (TextureOptions*)importsvec[id];

				static bool f0 = texoptions->filtering;
				ImGui::Checkbox("filtering", &f0);
				texoptions->filtering = f0;

				static bool f1 = texoptions->wrapping;
				ImGui::Checkbox("wrapping", &f1);
				texoptions->wrapping = f1;

				static bool f2 = texoptions->flipXY;
				ImGui::Checkbox("flip X/Y", &f2);
				texoptions->flipXY = f2;

				break;
			}
		
			}

			if (ImGui::Button("Import"))
			{
				App->scene_intro->WantToImport(importsvec[id]);
			}
			ImGui::End();

	}
}

void ModuleUI::DropTrget_In_Inspector(GameObject* node)
{
	ImVec2 low_point = ImGui::GetWindowContentRegionMin();
	ImVec2 top_point = ImGui::GetWindowContentRegionMax();

	low_point.x += ImGui::GetWindowPos().x;low_point.y += ImGui::GetWindowPos().y;
	top_point.x += ImGui::GetWindowPos().x;top_point.y += ImGui::GetWindowPos().y;
	
	ImRect space = { low_point.x, low_point.y, top_point.x, top_point.y };

	if (ImGui::BeginDragDropTargetCustom(space,10)) { //This case is when user drops an object but without doing reparent
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

		if (payload != nullptr)
		{
			if (payload->IsDataType(DROP_ID_HIERARCHY_NODES))
			{
				GameObject* obj = *(GameObject**)payload->Data;

				if (obj != nullptr) 
				{
					ChangeParent(obj, App->scene_intro->scene);
				}

			}
		}
		ImGui::EndDragDropTarget();
	}

}

void ModuleUI::AssetsHierarchyTree(AssetNode* node)
{
	ImGuiTreeNodeFlags node_flags = /*ImGuiTreeNodeFlags_DefaultOpen | */ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	int node_clicked = -1;

	std::string file, extension;
	App->file_system->SplitFilePath(node->owner->GetLibraryFile(), &file, &extension);
	file.append(extension);
	node->owner->name = file.c_str();

	const char* GameObjname = node->owner->name;

	if (node->is_selected == true)
	{
		node_flags += ImGuiTreeNodeFlags_Selected;
	}



	bool open = ImGui::TreeNodeEx(GameObjname, node_flags);

	if (ImGui::IsItemClicked())
	{
		DeactivateAssets();
		node->is_selected = true;
		selectedAsset = node;
	}

	if (open)
	{
		ImGui::TreePop();
	}

}

void ModuleUI::FolderHierarchyTree(FolderNode* node)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	int node_clicked = -1;


	if (selectedFolder == node)
	{
		node_flags += ImGuiTreeNodeFlags_Selected;
	}
	if (node->childs.empty())
	{
		node_flags += ImGuiTreeNodeFlags_Leaf;
	}

	bool open = ImGui::TreeNodeEx(node->path.c_str(), node_flags);

	if (ImGui::IsItemClicked())
	{

		selectedFolder = node;
		currentDirectory = selectedFolder->path;
		App->file_system->checkDirectoryFiles(currentDirectory.c_str(), &FilesInDir);
		SortFilesinDir();
	}

	if (open)
	{
		for (int i = 0; i < node->childs.size(); i++)
		{
			FolderHierarchyTree(node->childs[i]);
		}

		ImGui::TreePop();
	}
}

void ModuleUI::ChangeParent(GameObject* obj, GameObject* nextOwner)
{
	if (obj != nullptr && nextOwner != nullptr) {

		//obj->parent->to_delete = true;

		for (int i = 0; i < obj->parent->childs.size(); i++)
		{
			if (obj->parent->childs[i] == obj)
			{
				obj->parent->childs.erase(obj->parent->childs.begin() + i);
				i--;
				
			}
		}
		obj->parent = nextOwner;
		nextOwner->childs.push_back(obj);
	}
}

void ModuleUI::DeactivateGameObjects(GameObject* father)
{
	father->isSelected = false;
	for (int i = 0; i < father->childs.size(); i++)
	{
		DeactivateGameObjects(father->childs[i]);
	}
}

void ModuleUI::DeactivateAssets()
{
	selectedAsset = nullptr;
	for (int i = 0; i < assets.size(); i++)
	{
		assets[i]->is_selected = false;
	}
}

void ModuleUI::SortFilesinDir()
{
	vector< UiFile*> NewVec;
	for (int i = 0; i < FilesInDir.size(); i++)
	{
		if (FilesInDir[i]->extension == "")
		{
			NewVec.push_back(FilesInDir[i]);
		}
	}
	for (int i = 0; i < FilesInDir.size(); i++)
	{
		if (FilesInDir[i]->extension == "json")
		{
			NewVec.push_back(FilesInDir[i]);
		}
	}

	for (int i = 0; i < FilesInDir.size(); i++)
	{
		std::string path = FilesInDir[i]->file;
		path += FilesInDir[i]->extension;
		ResourceType type = App->ResManager->ReturnType(path.c_str());
		if(type == ResourceType::MODEL)
		NewVec.push_back(FilesInDir[i]);
		
	}

	for (int i = 0; i < FilesInDir.size(); i++)
	{
		std::string path = FilesInDir[i]->file;
		path += FilesInDir[i]->extension;
		ResourceType type = App->ResManager->ReturnType(path.c_str());
		if (type == ResourceType::TEXTURE)
			NewVec.push_back(FilesInDir[i]);

	}

	for (int i = 0; i < FilesInDir.size(); i++)
	{
		std::string path = FilesInDir[i]->file;
		path += FilesInDir[i]->extension;
		ResourceType type = App->ResManager->ReturnType(path.c_str());
		if (type == ResourceType::MESH)
			NewVec.push_back(FilesInDir[i]);

	}

	for (int i = 0; i < FilesInDir.size(); i++)
	{
		if (FilesInDir[i]->extension == "meta")
		{
			NewVec.push_back(FilesInDir[i]);
		}
	}


	FilesInDir = NewVec;

}
void ModuleUI::Change_Window_size(Vec2 newSize)
{
	//Get window size
	win_size = newSize;
	float offset = 10.0f;
	//Creates window size parameter
	image_size = App->window->windowSize ;
	if (image_size.x > win_size.x - offset)
	{image_size /= (image_size.x / (win_size.x - offset));}
	if (image_size.y > win_size.y - offset)
	{image_size /= (image_size.y / (win_size.y - offset));}
	image_offset = Vec2(win_size.x - (offset/2) - image_size.x, win_size.y - (offset / 2) - image_size.y);
	image_offset = image_offset / 2;
}


void ModuleUI::GuizmoUI() 
{
	
	GameObject* gameObject = App->scene_intro->selectedObj;
	using_gizmo = false;

	if (gameObject != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);

		float4x4 view = App->camera->cameraComp->frustrum.ViewMatrix();
		view.Transpose();
		
		float4x4 projection = App->camera->cameraComp->frustrum.ProjectionMatrix();
		projection.Transpose();

		float4x4 modelProjection = transform->global_transform;
		modelProjection.Transpose();

		ImGuizmo::SetDrawlist();
		cornerPos = Vec2(imgcorner.x, App->window->windowSize.y - imgcorner.y - image_size.y);
		ImGuizmo::SetRect(imgcorner.x, cornerPos.y, image_size.x, image_size.y);
		float4x4 InverseGlobal = modelProjection.Inverted();

		ImGuizmo::Manipulate(view.ptr(), projection.ptr(),guizmo_type, guizmo_mode, modelProjection.ptr());
		if (ImGuizmo::IsUsing())
		{
			using_gizmo = true;
			float4x4 MovementMatrix;
			MovementMatrix = InverseGlobal.Transposed() * modelProjection.Transposed();
			transform->local_transform = transform->local_transform * MovementMatrix;

			float3 translation, scaling;
			vec3 euler;
			Quat rotation;

			transform->local_transform.Decompose(translation, rotation, scaling);
			transform->pos.Set(translation.x, translation.y, translation.z);
			transform->rot.Set(rotation.x, rotation.y, rotation.z, rotation.w);
			transform->scale.Set(scaling.x, scaling.y, scaling.z);
			transform->using_guizmo = true;

		}
	}
}

void ModuleUI::ControlsGuizmo()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) 
	{
		guizmo_mode = ImGuizmo::MODE::LOCAL;
	} 
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) 
	{
		guizmo_mode = ImGuizmo::MODE::WORLD;
	} 
}

void ModuleUI::ChangePhysicsWindow()
{
	if (FisicsConfig_open == true)
	{
		ImGui::Begin("Physics Config", &FisicsConfig_open);

		
		float t = App->Physics->gravity;
		ImGui::SetNextItemWidth(200);
		ImGui::DragFloat("Gravity", &t,0.5f, -100,+100);
		if (ImGui::IsItemActive())
		{
			App->Physics->gravity = t;
			 btVector3 gravity = btVector3(0.0f, t, 0.0f);
			App->Physics->world->setGravity(gravity);
		}

		ImGui::End();
	}
}

void ModuleUI::RightClick_Assets_Menu(const char* path)
{
	if (ImGui::MenuItem("Delete"))
	{
		std::string meta = path;
		meta += ".meta";
		uint id = 0;
		ResourceType type = ResourceType::UNKNOWN;
		std::string Assets;
		std::string Library;
		uint timestamp1, timestamp2;

		App->serializer->LoadMeta(meta.c_str(), &id, &type, &Assets, &Library, nullptr, &timestamp1);
		if (type != ResourceType::UNKNOWN)
		{
			if (type != ResourceType::MODEL)
			{
				Library = App->file_system->substractPrefix(Library);
				meta = App->file_system->substractPrefix(meta);

				App->file_system->RemoveFile(Library.c_str());
				App->file_system->RemoveFile(meta.c_str());
			}
			else
			{
				std::vector<std::string> libpaths;
				App->serializer->LoadLibPathsFromModel(Library.c_str(), &libpaths);
				for (int i = 0; i < libpaths.size(); i++)
				{
					libpaths[i] = App->file_system->substractPrefix(libpaths[i]);
					App->file_system->RemoveFile(libpaths[i].c_str());
				}

				meta = App->file_system->substractPrefix(meta);
				App->file_system->RemoveFile(meta.c_str());
				Library = App->file_system->substractPrefix(Library);
				App->file_system->RemoveFile(Library.c_str());
			}

			Assets = App->file_system->substractPrefix(Assets);
			App->file_system->RemoveFile(Assets.c_str());
			currentDirectory = "Assets";
			App->file_system->checkDirectoryFiles(currentDirectory.c_str(), &FilesInDir);
			SortFilesinDir();
		}
	}
	if (ImGui::MenuItem("AddToScene"))
	{
		std::string meta = path;
		meta += ".meta";
		uint UID = App->ResManager->FindInAssets(path);
		if (UID == 0)
		{
			App->ResManager->ImportFileStep1(path);
		}
		if (UID != 0)
		{
			Resource* NewResource = App->ResManager->RequestResource(UID);
			
			switch (NewResource->GetType())
			{
			case ResourceType::MODEL:
				App->serializer->LoadModel(NewResource);
				break;
			case ResourceType::TEXTURE:
				App->FBX->ChangeTexture(NewResource);
				break;
			case ResourceType::MESH:
				App->FBX->ChangeMesh(NewResource);
				break;
		}

		
		}
	}
		
	

}
void ModuleUI::Change_Visibility_BoundingBoxes(GameObject* node,bool visibility)
{
	node->displayAABB = visibility;
	for (int i = 0; i < node->childs.size(); i++)
	{
		Change_Visibility_BoundingBoxes(node->childs[i], visibility);
	}
}



ImportOptions::ImportOptions()
{
	path = "";
	type = importType::UNKNOWN;
}

ImportOptions::ImportOptions(const char* importpath)
{
	path = importpath;
	type = importType::UNKNOWN;
}
ImportOptions::~ImportOptions()
{

}

ModelOptions::ModelOptions(const char* importpath)
{
	type = importType::MODEL;
	path = importpath;
	GlobalScale = 1;
	axis = false;
	ignoreCameras = false;
}
ModelOptions::~ModelOptions() {

}

TextureOptions::TextureOptions(const char* importpath)
{
	type = importType::TEXTURE;
	path = importpath;
	filtering = false;
	wrapping = false;
	flipXY = false;
}
TextureOptions::~TextureOptions() {

}
