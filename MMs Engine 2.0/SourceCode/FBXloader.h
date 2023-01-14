#pragma once
#include "Module.h"
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"


#define checkImageWidth 64
#define checkImageHeight 64


class GameObject;
class ComponentMaterial;
class ComponentMesh;
class ComponentTransform;
class Resource;
class ResourceModel;
class ResourceMesh;
class ResourceTexture;
class ImportOptions;
struct aiScene;
struct aiNode;

namespace MaterialImporter
{
	void Import(char* buffer, uint fileSize);
	void Save(char** fileBuffer, const char* path, std::string* newpath);
	void Load(const char* fileBuffer, uint size, ComponentMaterial* ourMaterial);
	void Load(const char* fileBuffer, uint size, ResourceTexture* resourceTexture);

}

namespace MeshImporter
{
	void Save(ComponentMesh* mesh, std::string* path, const char* name);
	void Load(char* fileBuffer, uint size, ComponentMesh* mesh);
	void Load(char* fileBuffer, uint size, ResourceMesh* ResourceMesh);
}
struct mesh
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_normals = 0; // normals in VRAM
	uint num_normals = 0;
	float* normals = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_tex;
	uint num_tex;
	float* texCoords;

	uint texbuffer;
	bool hastexture;
};



class FBXloader : public Module
{
public:
	FBXloader(Application* app, bool start_enabled = true);
	~FBXloader();

	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();
	uint FillArrayBuffer(uint size, float* array);
	uint FillElementArrayBuffer(uint size, uint* array);
	uint LoadTexBuffer(const char* path);
	bool LoadFBX(const char* buffer, uint size);
	bool LoadFBX(const char* buffer, uint size, ResourceModel* resource, ImportOptions* options = nullptr);

	void LoadNode(const aiScene* scene, aiNode* node, GameObject* father = nullptr);
	void LoadNode(const aiScene* scene, aiNode* node, ResourceModel* model, GameObject* father = nullptr, ImportOptions* options = nullptr);

	void PrintMeshes();
	void ChangeTexture(const char* path);
	void ChangeTexture(Resource* resource);
	void ChangeMesh(const char* path);
	void ChangeMesh(Resource* resource);

	void CreateAABB(ComponentMesh* NewMesh);

private:

public:
	int numberGO;
	vector<mesh*> meshes;
	int texture_w;
	int texture_h;
};

