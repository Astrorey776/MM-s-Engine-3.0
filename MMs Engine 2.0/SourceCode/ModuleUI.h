#pragma once
#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include<iostream> 
#include<string>
#include "Vec2.h"
#include "ImGuizmo.h"

using namespace std;
class GameObject;
class Resource;
class UiFile;

enum class importType
{
	UNKNOWN,
	MODEL,
	TEXTURE,
};
class ImportOptions
{
public:
	ImportOptions();
	ImportOptions(const char* importpath);
	~ImportOptions();
	Resource* reference;
	importType type;
	std::string path;
};

class ModelOptions: public ImportOptions
{
public:
	ModelOptions(const char* importpath);
	~ModelOptions();
	int GlobalScale;
	bool axis;
	bool ignoreCameras;
};

class TextureOptions : public ImportOptions
{
public:
	TextureOptions(const char* importpath);
	~TextureOptions();
	bool filtering;
	bool wrapping;
	bool flipXY;
};

class AssetNode
{
public:
	AssetNode(Resource* res)
	{
		owner = res;
		is_selected = false;
		to_delete = false;
	}
	~AssetNode(){
	
	}

public: 
	Resource* owner;
	bool is_selected = false;
	bool to_delete;
};

class FolderNode
{
public:

	FolderNode(string res, FolderNode* Nodeparent)
	{
		path = res;
		parent = Nodeparent;
		if (parent != nullptr) 
		{
			parent->childs.push_back(this);
		}
	}
	~FolderNode() {

	}

public:
	std::string path;
	FolderNode* parent;
	std::vector<FolderNode*> childs;
};

class TreeNode
{
public:
	TreeNode(GameObject* obj)
	{
		object = obj;
		isSelected = false;
	}
	~TreeNode();
public:
	GameObject* object;
	bool isSelected;
};
class ModuleUI : public Module
{
public:
	ModuleUI(Application* app, bool start_enabled = true);
	~ModuleUI();

	bool Init();

	update_status Update(float dt);
	update_status PostUpdate(float dt);

	ImportOptions* CreateImportObject(const char* importpath, importType type);
	bool CleanUp();

	void AboutMenu(bool window);
	void StoreLog(const char* message);
	void ShowAppinDockSpace(bool* p_open = NULL);

	void AssetsTree(); //hierarchy window

	void ImportWindow(); //import window
	void ChangePhysicsWindow();

	void SortFilesinDir();
	void Change_Window_size(Vec2 newSize);

	void GameObjectHierarchyTree(GameObject* node, int id);
	void Change_Visibility_BoundingBoxes(GameObject* node,bool visibility);
	void AssetsHierarchyTree(AssetNode* node);
	void FolderHierarchyTree(FolderNode* node);

	void DeactivateGameObjects(GameObject* father);
	void DeactivateAssets();

	void ChangeParent(GameObject* obj, GameObject* nextOwner);
	void DropTrget_In_Inspector(GameObject* node);
	void SelectGameObject(GameObject* node);
	void GuizmoUI();
	void ControlsGuizmo();
	void RightClick_Assets_Menu(const char* path);

	vec3 ReturnLookAtCamera(vec3 direction);
	AssetNode* createAssetNode(Resource* resource);
public:
	ImGuiIO* io;
	bool show_demo_window;
	bool show_console;
	ImVec4 clear_color;
	bool show_another_window;
	bool active2 = false;
	bool show_About;
	bool resizable_bool = false;
	bool border_bool;
	bool Wireframe_bool;
	 bool Hierarchy_open;
	 bool Assetstree_open;
	 bool Inspector_open;
	 bool Config_Camera_open;
	 bool Console_open;
	 bool FisicsConfig_open;
	 bool importWindow;
	vector <float> fpsecond;
	bool show_Configuration;
	int max_fps = 61;
	int Screen_i = 0;
	int Screen_e = 0;
	bool* open_docking;
	ImVec2 MouseVelocity;
	ImVec2 windowSize = { 0, 0 };
	Vec2 win_size;
	Vec2 image_offset;
	Vec2 img_corner;
	Vec2 image_size;
	ImGuiWindowClass* windowClass = nullptr;
	vec3 direction_camera;
	ImVec2 winSize;
	ImVec2 winPos;
	Vec2 cornerPos;
	Vec2 imgcorner;
	ImGuizmo::OPERATION guizmo_type = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizmo_mode = ImGuizmo::MODE::LOCAL;
	bool LocalGuizmo;
	bool WorldGuizmo;
	bool using_gizmo;
	vector<AssetNode*> assets;
	int cameras;
	int empty_GameObjects;
	vector<ImportOptions*> importsvec;
	std::string clickedAsset;
	vector< UiFile*> FilesInDir;
	std::string currentDirectory;
	vector<string> consoleOutput;
private:
	int c1;
	float width = 1324;
	float height = 768;
	AssetNode* selectedAsset;
	FolderNode* selectedFolder;
	std::string currentFolderDirectory;
	FolderNode* FoldersInDir;
};

