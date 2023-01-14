#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include <string>

class GameObject;
class Constraint;
class Primitive;
class ComponentCamera;
class ImportOptions;
class PhysBody3D;

typedef unsigned char GLubyte;
typedef unsigned int GLuint;

#define checkImageWidth 64
#define checkImageHeight 64

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

	void UpdateGameObject(GameObject* parent, float dt);
	void SetDelete(GameObject* parent);
	void DeleteSceneObjects(GameObject* parent);
	bool DeleteGameObject(GameObject* parent);
	void SaveScene(GameObject* parent);
	void SaveScene();
	void Camera_Editor_Window(ComponentCamera* camera);
	void WantToImport(ImportOptions* options);
	void ManageDropEvent(std::string file_path);
	GameObject* ReturnGameObject(uint UID, GameObject* parent);
	void OnCollision(PhysBody3D* body1, PhysBody3D* body2) override;

public:

	uint my_indices = 0;
	uint my_vertex = 0;
	uint my_indices2 = 0;
	uint my_vertex2 = 0;
	int num_vertices;
	int num_indices;
	int num_vertices2;
	int num_indices2;
	float vert[24];
	uint index[36];
	float vert2[24];
	uint index2[36];

	//vector<GameObject*> gameObjects;
	GameObject* scene;
	GameObject* selectedObj;

	ComponentCamera* culling;
	GLubyte checkImage[checkImageHeight][checkImageWidth][4];
	uint texName;
	bool wanttoload;
	uint FolderIco;
	uint FolderGoBack;
	uint FbxIcon;
	uint PngIcon;
	uint TgaIcon;
	uint MetaIcon;
	uint MeshIcon;
	uint DdsIcon;
	uint JsonIcon;
	uint ModelIcon;

	bool CreatingJoint;
	GameObject* JointObj1;
	GameObject* JointObj2;
	Primitive* cube1;
	Primitive* spherecamera;
	float3 distance;
	float3 axis1;
	float3 axis2;
	vector<Constraint*> constraints;
};
