#include "Globals.h"
#include "Application.h"
#include "serializer.h"
#include "parson/parson.h"
#include "FileSystem.h"
#include "GameObject.h"




Serializer::Serializer(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}
Serializer::~Serializer()
{

}

bool Serializer::Start()
{

	bool ret = true;
	LOG("starting serializer");
	//serialization_example();
	//	int num = get_Number("Assets/library/TEST.json", "age");
	CreateNewScene();


//	arrayExample();
	//get_Array("Assets/library/TEST.json");
//	LOG("%d", num);

	return ret;
}

update_status Serializer::PostUpdate(float dt) 
{
	return UPDATE_CONTINUE;

}
bool Serializer::CleanUp() 
{
	return true;
}

double Serializer::get_Number(const char* file, const char* name)
{
	JSON_Value* root_value;
	JSON_Object* object;
	root_value = json_parse_file(file);
	object = json_value_get_object(root_value);
	
	double number = 0;

	if (json_object_has_value_of_type(object, name, JSONNumber))
		number = json_object_get_number(object, name);

	json_value_free(root_value);
	return number;
}
const char* Serializer::get_String(const char* file, const char* name)
{
	JSON_Value* root_value;
	JSON_Object* object;
	root_value = json_parse_file(file);
	object = json_value_get_object(root_value);

	std::string newstring;

	if (json_object_has_value_of_type(object, name, JSONString))
		newstring = json_object_get_string(object, name);

	json_value_free(root_value);
	return newstring.c_str();
}
void Serializer::get_Array(const char* file)
{
	JSON_Value* root_value;
	JSON_Object* main_object;
	JSON_Array* main_array;

	root_value = json_parse_file(file);

	main_object = json_value_get_object(root_value);
	main_array = json_object_get_array(main_object, "Game Objects");


	for (int i = 0; i < json_array_get_count(main_array); i++) {
		JSON_Object* obj_in_array = json_array_get_object(main_array, i);
		JSON_Array* array_in_obj = json_object_get_array(obj_in_array, "Components");
	
		LOG("%f", json_object_get_number(obj_in_array, "UID"));
		LOG("%f", json_object_get_number(obj_in_array, "ParentUID"));
		LOG("%s", json_object_get_string(obj_in_array, "Name"));

		for (int j = 0; j < json_array_get_count(array_in_obj); j++)
		{
			JSON_Object* obj_in_array_in_obj = json_array_get_object(array_in_obj, j);
			LOG("%s", json_object_get_string(obj_in_array_in_obj, "Type"));
			LOG("%s", json_object_get_string(obj_in_array_in_obj, "Path"));

		}
	}

}


void Serializer::CreateNewScene()
{
	//creating a Json_Array
	 root_value = json_value_init_object();
	 root_object = json_value_get_object(root_value);
	 branch = json_value_init_array();
	 GameObjects = json_value_get_array(branch);
	 branch2 = json_value_init_array();
	 Constraints = json_value_get_array(branch2);
	 json_object_set_value(root_object, "Constraints", branch2);

	 json_object_set_value(root_object, "Game Objects", branch);

}
JSON_Object* Serializer::AddObjectToArray(JSON_Array* obj)
{
	JSON_Value* ObjectValue = json_value_init_object();
	JSON_Object* Object = json_value_get_object(ObjectValue);
	
	json_array_append_value(obj, ObjectValue);
	return Object;

}

void Serializer::AddVec3(JSON_Array* obj, float x, float y, float z)
{
	json_array_append_number(obj, x);
	json_array_append_number(obj, y);
	json_array_append_number(obj, z);
}
void Serializer::AddVec4(JSON_Array* obj, float x, float y, float z, float w)
{
	json_array_append_number(obj, x);
	json_array_append_number(obj, y);
	json_array_append_number(obj, z);
	json_array_append_number(obj, w);

}

void Serializer::SaveScene(const char* name)
{
	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(root_value);
	size_t size = sprintf(serialized_string, "%s", serialized_string);

	App->file_system->Save(name, serialized_string, size, false);
	json_free_serialized_string(serialized_string);
}


void Serializer::SaveValueAsFile(JSON_Value* root, const char* name, std::string directory)
{
	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(root);
	size_t size = sprintf(serialized_string, "%s", serialized_string);
	directory = directory + name;
	App->file_system->Save(directory.c_str(), serialized_string, size, false);
	json_free_serialized_string(serialized_string);
}

void Serializer::LoadScene(const char* path)
{
	JSON_Value* root_value;
	JSON_Object* main_object;
	JSON_Array* object_array;
	JSON_Array* constraints_array;

	root_value = json_parse_file(path);
	if (root_value != NULL)
	{
	

		main_object = json_value_get_object(root_value);
		object_array = json_object_get_array(main_object, "Game Objects");


		for (int i = 0; i < json_array_get_count(object_array); i++) {
			JSON_Object* obj_in_array = json_array_get_object(object_array, i);
			JSON_Array* component_array = json_object_get_array(obj_in_array, "Components");
			JSON_Array* JsonTrans = json_object_get_array(obj_in_array, "Translation");
			JSON_Array* JsonScale = json_object_get_array(obj_in_array, "Scale");
			JSON_Array* JsonRot = json_object_get_array(obj_in_array, "Rotation");

			int UID = json_object_get_number(obj_in_array, "UID");
			int parentUID = json_object_get_number(obj_in_array, "ParentUID");
			const char* name = json_object_get_string(obj_in_array, "Name");

			GameObject* object = App->scene_intro->CreateGameObject(name, nullptr);
			tempvector.push_back(object);
			object->UID = UID;
			object->parentUID = parentUID;
			if (object->parentUID == object->UID)
			{
				LCG();
				LCG rand;
				object->UID = rand.Int();
			}

			ComponentTransform* NewTrans = (ComponentTransform*)object->CreateComponent(ComponentType::TRANSFORM);
			NewTrans->pos.x = json_array_get_number(JsonTrans, 0);
			NewTrans->pos.y = json_array_get_number(JsonTrans, 1);
			NewTrans->pos.z = json_array_get_number(JsonTrans, 2);

			NewTrans->scale.x = json_array_get_number(JsonScale, 0);
			NewTrans->scale.y = json_array_get_number(JsonScale, 1);
			NewTrans->scale.z = json_array_get_number(JsonScale, 2);

			NewTrans->rot.x = json_array_get_number(JsonRot, 0);
			NewTrans->rot.y = json_array_get_number(JsonRot, 1);
			NewTrans->rot.z = json_array_get_number(JsonRot, 2);
			NewTrans->rot.w = json_array_get_number(JsonRot, 3);

			NewTrans->local_transform = float4x4::FromTRS(NewTrans->pos, NewTrans->rot, NewTrans->scale);
			NewTrans->local_transform;

			NewTrans->global_transform = NewTrans->AcumulateparentTransform();

			for (int j = 0; j < json_array_get_count(component_array); j++)
			{
				JSON_Object* obj_in_array_in_obj = json_array_get_object(component_array, j);
				std::string type = json_object_get_string(obj_in_array_in_obj, "Type");
				uint componentUID = json_object_get_number(obj_in_array_in_obj, "UID");
				const char* componentpath = json_object_get_string(obj_in_array_in_obj, "Path");


				if (type == "Mesh")
				{
					App->ResManager->FindInLibrary(componentpath, componentUID);
					ResourceMesh* NewMeshResource;
					NewMeshResource = (ResourceMesh*)App->ResManager->RequestResource(componentUID);
					if (NewMeshResource != nullptr)
					{
						ComponentMesh* NewMesh = (ComponentMesh*)object->CreateComponent(ComponentType::MESH);
						NewMesh->reference = NewMeshResource;
						NewMesh->num_vertex = NewMeshResource->num_vertex;
						NewMesh->num_tex = NewMeshResource->num_tex;
						NewMesh->num_normals = NewMeshResource->num_normals;
						NewMesh->num_index = NewMeshResource->num_index;
						NewMesh->vertex = NewMeshResource->vertex;
						NewMesh->texCoords = NewMeshResource->texCoords;
						NewMesh->normals = NewMeshResource->normals;
						NewMesh->index = NewMeshResource->index;
						NewMesh->id_vertex = NewMeshResource->id_vertex;
						NewMesh->id_tex = NewMeshResource->id_tex;
						NewMesh->id_normals = NewMeshResource->id_normals;
						NewMesh->id_index = NewMeshResource->id_index;


						NewMesh->SetAABB();
						//	NewMeshResource->unloadResource();
					}
					else
						LOG("Error loading model resource meshes");

				}
				else if (type == "texture")
				{
					App->ResManager->FindInLibrary(componentpath, componentUID);

					ResourceTexture* NewTexResource = (ResourceTexture*)App->ResManager->RequestResource(componentUID);
					if (NewTexResource != nullptr)
					{
						ComponentMaterial* NewTex = (ComponentMaterial*)object->CreateComponent(ComponentType::MATERIAL);
						NewTex->reference = NewTexResource;

						NewTex->texbuffer = NewTexResource->texbuffer;
						NewTex->texture_h = NewTexResource->texture_h;
						NewTex->texture_w = NewTexResource->texture_w;
						NewTex->texture_path = NewTexResource->GetLibraryFile();
						if (NewTex->texbuffer != 0)
							NewTex->hastexture = true;
						//	NewMeshResource->unloadResource();
					}
					else
						LOG("Error loading model resource texture");

				}
				else if (type == "Collider")
				{
					ComponentTransform* myTrans = (ComponentTransform*)object->GetComponent(ComponentType::TRANSFORM);
					myTrans->should_update = true;
				

					ComponentType type = ComponentType::COLLIDER;

					if (object->GetComponent(type) == nullptr)
						object->CreateComponent(type);

					Collider* collider = (Collider*)object->GetComponent(ComponentType::COLLIDER);

					std::string Collidertype = json_object_get_string(obj_in_array_in_obj, "ColliderType");
					float mass = json_object_get_number(obj_in_array_in_obj, "Mass");
					if (Collidertype == "Box")
						collider->collidertype = ColliderType::BOX;

					else if (Collidertype == "Sphere")
						collider->collidertype = ColliderType::SPHERE;

					else if (Collidertype == "Capsule")
						collider->collidertype = ColliderType::CAPSULE;
					else if (Collidertype == "None")
						LOG("Collider type unknown");

					collider->body.SetBody(object, mass, collider->collidertype);
					bool sensor = json_object_get_number(obj_in_array_in_obj, "Sensor");
					if(sensor)
					collider->body.SetAsSensor(sensor);
					JSON_Array* JsonTrans = json_object_get_array(obj_in_array_in_obj, "Translation");
					JSON_Array* JsonScale = json_object_get_array(obj_in_array_in_obj, "Scale");
					JSON_Array* JsonRot = json_object_get_array(obj_in_array_in_obj, "Rotation");
					float3 pos, scale;
					Quat rot;
					pos.x = json_array_get_number(JsonTrans, 0);
					pos.y = json_array_get_number(JsonTrans, 1);
					pos.z = json_array_get_number(JsonTrans, 2);

					scale.x = json_array_get_number(JsonScale, 0);
					scale.y = json_array_get_number(JsonScale, 1);
					scale.z = json_array_get_number(JsonScale, 2);

					rot.x = json_array_get_number(JsonRot, 0);
					rot.y = json_array_get_number(JsonRot, 1);
					rot.z = json_array_get_number(JsonRot, 2);
					rot.w = json_array_get_number(JsonRot, 3);

					btVector3 size;
					size.setValue(scale.x, scale.y, scale.z);

					collider->body.GetBody()->getCollisionShape()->setLocalScaling(size);
					collider->body.TransformMatrix = float4x4::FromTRS(pos, rot, scale);
					collider->body.SetTransform(collider->body.globalTransform);

					
				}

			}
		}
		sortScene();

		App->scene_intro->constraints.clear();
		constraints_array = json_object_get_array(main_object, "Constraints");
		for (int i = 0; i < json_array_get_count(constraints_array); i++) {
			JSON_Object* obj_in_array = json_array_get_object(constraints_array, i);
			JSON_Array* JsonTrans = json_object_get_array(obj_in_array, "Distance");
			
			uint UID1 = json_object_get_number(obj_in_array, "Obj1");
			uint UID2 = json_object_get_number(obj_in_array, "Obj2");
			std::string type = json_object_get_string(obj_in_array, "Type");

			float x = json_array_get_number(JsonTrans, 0);
			float y = json_array_get_number(JsonTrans, 1);
			float z = json_array_get_number(JsonTrans, 2);
			GameObject* objA = nullptr;
			GameObject* objB = nullptr;
			btVector3 distance;
			distance.setValue(x, y, z);

			for (int j = 0; j < tempvector.size(); j++)
			{
				if (tempvector[j]->UID == UID1)
					objA = tempvector[j];
				if (tempvector[j]->UID == UID2)
					objB = tempvector[j];

			}
			if (type == "P2P")
			{
				App->Physics->AddConstraintP2P(objA, objB, distance);

			}
			else if (type == "Hinge")
			{
				btVector3 AxisinA;
				JSON_Array* Axis = json_object_get_array(obj_in_array, "Axis1");

				float x = json_array_get_number(Axis, 0);
				float y = json_array_get_number(Axis, 1);
				float z = json_array_get_number(Axis, 2);
				AxisinA.setValue(x, y, z);
				btVector3 AxisinB;
				JSON_Array* Axis2 = json_object_get_array(obj_in_array, "Axis1");

				x = json_array_get_number(Axis2, 0);
				y = json_array_get_number(Axis2, 1);
				z = json_array_get_number(Axis2, 2);

				AxisinB.setValue(x, y, z);
				App->Physics->AddConstraintHinge(objA, objB, distance,AxisinA, AxisinB );

			}
			else if (type == "Slider")
			{
				App->Physics->AddConstraintSlider(objA, objB, distance);

			}
			else if (type == "Cone")
			{
				App->Physics->AddConstraintCone(objA, objB, distance);

			}
			
			
		
			

		}
	/*	for (int i = 0; i < tempvector.size(); i++)
		{
			LCG();
			LCG rand;
			tempvector[i]->UID = rand.Int();

		}*/
		tempvector.clear();

	}
	

}

void Serializer::sortScene() {
	for (int i = 0; i < tempvector.size(); i++)
	{
		/*if (tempvector[i]->UID == tempvector[i]->parentUID)
		{
			LOG("Error loading object in scene")
			continue;
		}*/
		for (int j = 0; j < tempvector.size(); j++)
		{
			if (tempvector[i]->parentUID == tempvector[j]->UID)
			{
				tempvector[i]->parent = tempvector[j];
				tempvector[j]->childs.push_back(tempvector[i]);
				
			}
				
		}
		if (tempvector[i]->parent == nullptr)
		{
			tempvector[i]->parent = App->scene_intro->scene;
			App->scene_intro->scene->childs.push_back(tempvector[i]);
		}
	}


}

void Serializer::LoadModel(Resource* model)
{
	JSON_Value* root_value;
	JSON_Object* main_object;
	JSON_Array* main_array;

	root_value = json_parse_file(model->GetLibraryFile());

	main_object = json_value_get_object(root_value);
	main_array = json_object_get_array(main_object, "Game Objects");


	for (int i = 0; i < json_array_get_count(main_array); i++) {
		JSON_Object* obj_in_array = json_array_get_object(main_array, i);
		JSON_Array* component_array = json_object_get_array(obj_in_array, "Components");
		JSON_Array* JsonTrans = json_object_get_array(obj_in_array, "Translation");
		JSON_Array* JsonScale = json_object_get_array(obj_in_array, "Scale");
		JSON_Array* JsonRot = json_object_get_array(obj_in_array, "Rotation");

		int UID = json_object_get_number(obj_in_array, "UID");
		int parentUID = json_object_get_number(obj_in_array, "ParentUID");
		const char* name = json_object_get_string(obj_in_array, "Name");

		GameObject* object = App->scene_intro->CreateGameObject(name, nullptr);
		tempvector.push_back(object);
		object->UID = UID;
		object->parentUID = parentUID;
		if (object->parentUID == object->UID)
		{
			LCG();
			LCG rand;
			object->UID = rand.Int();
		}

		ComponentTransform* NewTrans = (ComponentTransform*)object->CreateComponent(ComponentType::TRANSFORM);
		NewTrans->pos.x = json_array_get_number(JsonTrans, 0);
		NewTrans->pos.y = json_array_get_number(JsonTrans, 1);
		NewTrans->pos.z = json_array_get_number(JsonTrans, 2);

		NewTrans->scale.x = json_array_get_number(JsonScale, 0);
		NewTrans->scale.y = json_array_get_number(JsonScale, 1);
		NewTrans->scale.z = json_array_get_number(JsonScale, 2);

		NewTrans->rot.x = json_array_get_number(JsonRot, 0);
		NewTrans->rot.y = json_array_get_number(JsonRot, 1);
		NewTrans->rot.z = json_array_get_number(JsonRot, 2);
		NewTrans->rot.w = json_array_get_number(JsonRot, 3);

		NewTrans->local_transform = float4x4::FromTRS(NewTrans->pos, NewTrans->rot, NewTrans->scale);
		NewTrans->local_transform;

		for (int j = 0; j < json_array_get_count(component_array); j++)
		{
			JSON_Object* obj_in_array_in_obj = json_array_get_object(component_array, j);
			std::string type = json_object_get_string(obj_in_array_in_obj, "Type");
			uint componentUID = json_object_get_number(obj_in_array_in_obj, "Resource UID");
			const char* componentpath = json_object_get_string(obj_in_array_in_obj, "Path");


			if (type == "Mesh")
			{
				App->ResManager->FindInLibrary(componentpath, componentUID);
				ResourceMesh* NewMeshResource;
				NewMeshResource = (ResourceMesh*)App->ResManager->RequestResource(componentUID);
				if (NewMeshResource != nullptr)
				{
					ComponentMesh* NewMesh = (ComponentMesh*)object->CreateComponent(ComponentType::MESH);
					NewMesh->reference = NewMeshResource;
					NewMesh->num_vertex = NewMeshResource->num_vertex;
					NewMesh->num_tex = NewMeshResource->num_tex;
					NewMesh->num_normals = NewMeshResource->num_normals;
					NewMesh->num_index = NewMeshResource->num_index;
					NewMesh->vertex = NewMeshResource->vertex;
					NewMesh->texCoords = NewMeshResource->texCoords;
					NewMesh->normals = NewMeshResource->normals;
					NewMesh->index = NewMeshResource->index;
					NewMesh->id_vertex = NewMeshResource->id_vertex;
					NewMesh->id_tex = NewMeshResource->id_tex;
					NewMesh->id_normals = NewMeshResource->id_normals;
					NewMesh->id_index = NewMeshResource->id_index;
					

					NewMesh->SetAABB();
				//	NewMeshResource->unloadResource();
				}
				else
					LOG("Error loading model resource meshes");
		
			}
			else if (type == "texture")
			{
				App->ResManager->FindInLibrary(componentpath, componentUID);
				ResourceTexture* NewTexResource = (ResourceTexture*)App->ResManager->RequestResource(componentUID);
				if (NewTexResource != nullptr)
				{
					ComponentMaterial* NewTex = (ComponentMaterial*)object->CreateComponent(ComponentType::MATERIAL);
					NewTex->reference = NewTexResource;

					NewTex->texbuffer = NewTexResource->texbuffer;
					NewTex->texture_h = NewTexResource->texture_h;
					NewTex->texture_w = NewTexResource->texture_w;
					NewTex->texture_path = NewTexResource->GetLibraryFile();

					if (NewTex->texbuffer != 0)
						NewTex->hastexture = true;
				//	NewMeshResource->unloadResource();
				}
				else
				LOG("Error loading model resource texture");
				
			}


		}
	}
	sortScene();
	for (int i = 0; i < tempvector.size(); i++)
	{
		LCG();
		LCG rand;
		tempvector[i]->UID = rand.Int();

	}
	tempvector.clear();
}

void Serializer::LoadLibPathsFromModel(const char* path, std::vector<std::string>* direvector)
{
	JSON_Value* root_value;
	JSON_Object* main_object;
	JSON_Array* main_array;

	root_value = json_parse_file(path);
	main_object = json_value_get_object(root_value);
	main_array = json_object_get_array(main_object, "Game Objects");


	for (int i = 0; i < json_array_get_count(main_array); i++) {
		JSON_Object* obj_in_array = json_array_get_object(main_array, i);
		JSON_Array* component_array = json_object_get_array(obj_in_array, "Components");

		for (int j = 0; j < json_array_get_count(component_array); j++)
		{
			JSON_Object* obj_in_array_in_obj = json_array_get_object(component_array, j);
			std::string type = json_object_get_string(obj_in_array_in_obj, "Type");
			const char* componentpath = json_object_get_string(obj_in_array_in_obj, "Path");


			if (type == "Mesh")
			{
				direvector->push_back(componentpath);

			}

		}
	}
	
}

bool Serializer::LoadMeta(const char* path, uint* uid, ResourceType* type, std::string* Assets, std::string* library, TextureOptions* options, uint* timestamp)
{
	bool ret = false;
	JSON_Value* value;
	JSON_Object* object;


	value = json_parse_file(path);
	if (value == NULL)
	{
		return ret;
	}
	ret = true;
	object = json_value_get_object(value);


	if (json_object_has_value_of_type(object, "UID", JSONNumber))
		*uid = json_object_get_number(object, "UID");
	if (timestamp != nullptr)
	{
		if (json_object_has_value_of_type(object, "Last modified", JSONNumber))
			*timestamp = json_object_get_number(object, "Last modified");
	}

	if (options != nullptr)
	{
		if (json_object_has_value_of_type(object, "Filtering", JSONNumber))
			options->filtering = json_object_get_number(object, "Filtering");

		if (json_object_has_value_of_type(object, "Wrapping", JSONNumber))
			options->filtering = json_object_get_number(object, "Wrapping");

		if (json_object_has_value_of_type(object, "FlipXY", JSONNumber))
			options->flipXY = json_object_get_number(object, "FlipXY");
	}
	std::string Type;

	if (json_object_has_value_of_type(object, "Type", JSONString))
		Type = json_object_get_string(object, "Type");
	

	if (Type == "3D Model")
		*type = ResourceType::MODEL;
	else if (Type == "Texture")
		*type = ResourceType::TEXTURE;

	if (json_object_has_value_of_type(object, "Asset Path", JSONString))
		*Assets = json_object_get_string(object, "Asset Path");
	if (json_object_has_value_of_type(object, "Library path", JSONString))
		*library = json_object_get_string(object, "Library path");

	//json_value_free(root_value);
	return ret;
}
void Serializer::AddFloat(JSON_Object* obj, const char* name, double value)
{
	json_object_set_number(obj, name, value);
}
void Serializer::AddString(JSON_Object* obj, const char* name, const char* string)
{
	json_object_set_string(obj, name, string);
}

JSON_Array* Serializer::AddArray(JSON_Object* obj, const char* name)
{
	JSON_Value* components = json_value_init_array();
	JSON_Array* componentsArray = json_value_get_array(components);

	json_object_set_value(obj, name, components);
	return componentsArray;
}

void Serializer::AddComponent(JSON_Array* componentsArray, Component* component, const char* path, uint UID)
{
	
	JSON_Value* leaf_value = json_value_init_object();
	JSON_Object* leaf_object = json_value_get_object(leaf_value);
	switch (component->type)
	{
		case ComponentType::NONE: 
			break;
		case ComponentType::MESH: 
		{ 
			AddString(leaf_object, "Type", "Mesh");
			AddString(leaf_object, "Path", path);
			AddFloat(leaf_object, "UID", UID);
			break;
		}
		case ComponentType::MATERIAL: { 
			AddString(leaf_object, "Type", "texture");
			AddString(leaf_object, "Path", path);
			AddFloat(leaf_object, "UID", UID);
			break; 
		}
		case ComponentType::TRANSFORM: { 

			break; 
		}
		case ComponentType::COLLIDER:
		{
			AddString(leaf_object, "Type", "Collider");

			Collider* collider = (Collider*)component;
			App->serializer->AddFloat(leaf_object, "Sensor", collider->body.IsSensor());

			switch (collider->body.type)
			{
			case ColliderType::NONE:
				AddString(leaf_object, "ColliderType", "None");
				break;
			case ColliderType::BOX:
				AddString(leaf_object, "ColliderType", "Box");
				break;
			case ColliderType::SPHERE:
				AddString(leaf_object, "ColliderType", "Sphere");
				break;
			case ColliderType::CAPSULE:
				AddString(leaf_object, "ColliderType", "Capsule");
				break;
			}
			AddFloat(leaf_object, "Mass", collider->body.BodyMass);
			float4x4 collider_trans = collider->body.TransformMatrix;
			
			float3 trans, scale;
			Quat rot;
			collider_trans.Decompose(trans, rot, scale);
			JSON_Array* JsonTrans = App->serializer->AddArray(leaf_object, "Translation");
			JSON_Array* JsonScale = App->serializer->AddArray(leaf_object, "Scale");
			JSON_Array* JsonRot = App->serializer->AddArray(leaf_object, "Rotation");
			AddVec3(JsonTrans, trans.x, trans.y, trans.z);
			AddVec3(JsonScale, scale.x, scale.y, scale.z);
			AddVec4(JsonRot, rot.x, rot.y, rot.z, rot.w);
			break;
		}

	}

	json_array_append_value(componentsArray, leaf_value);
	
}

void Serializer::AddResourceComponent(JSON_Array* componentsArray, ComponentType type, uint UID, const char* path)
{

	JSON_Value* leaf_value = json_value_init_object();
	JSON_Object* leaf_object = json_value_get_object(leaf_value);
	switch (type)
	{
	case ComponentType::NONE:
		break;
	case ComponentType::MESH:
	{
		AddString(leaf_object, "Type", "Mesh");
		AddFloat(leaf_object, "Resource UID", UID);
		AddString(leaf_object, "Path", path);
		break;
	}
	case ComponentType::MATERIAL: {
		AddString(leaf_object, "Type", "texture");
		AddFloat(leaf_object, "Resource UID", UID);
		AddString(leaf_object, "Path", path);
		break;
	}
	case ComponentType::TRANSFORM: {

		break;
	}

	}

	json_array_append_value(componentsArray, leaf_value);

}


