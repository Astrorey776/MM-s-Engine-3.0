#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "Module.h"
#include "Globals.h"

typedef struct json_object_t JSON_Object;
typedef struct json_array_t  JSON_Array;
typedef struct json_value_t  JSON_Value;

class Component;
class TextureOptions;
enum class ComponentType;
enum class ResourceType;

class Serializer : public Module
{
public:
	Serializer(Application* app, bool start_enabled = true);

	~Serializer();

	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateNewScene();
	JSON_Object* AddObjectToArray(JSON_Array* obj);
	void SaveScene(const char* name);

	void SaveValueAsFile(JSON_Value* root, const char* name, std::string directory = "");
	void LoadScene(const char* path);
	void LoadModel(Resource* model);
	bool LoadMeta(const char* path, uint* uid, ResourceType* type, std::string* Assets, std::string* library, TextureOptions* options = nullptr, uint* timestamp = nullptr);
	void LoadLibPathsFromModel(const char* path, std::vector<std::string>* direvector);
	void sortScene();
	void AddFloat(JSON_Object* obj, const char* name, double value);
	void AddString(JSON_Object* obj, const char* name, const char* string);
	void AddVec3(JSON_Array* obj, float x, float y, float z);
	void AddVec4(JSON_Array* obj, float x, float y, float z, float w);
	void AddComponent(JSON_Array* obj, Component* type, const char* path, uint UID = 0);
	void AddResourceComponent(JSON_Array* obj, ComponentType type, uint UID, const char* path);

	JSON_Array* AddArray(JSON_Object* obj, const char* name);

	double get_Number(const char* file, const char* name);
	const char* get_String(const char* file, const char* name);

	void get_Array(const char* file);



public:
	JSON_Value* root_value;
	JSON_Object* root_object;
	JSON_Value* branch;
	JSON_Value* branch2;
	JSON_Array* GameObjects;
	JSON_Array* Constraints;
	std::vector<GameObject*> tempvector;
};
#endif