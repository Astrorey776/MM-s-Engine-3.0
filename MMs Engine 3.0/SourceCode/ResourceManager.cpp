#include "Application.h"
#include "ResourceManager.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "parson/parson.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include <algorithm>    

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	
}


ResourceManager::~ResourceManager()
{
	
}

bool ResourceManager::Init()
{
	// Determine if the PhysicsFS library is initialized, we can check it for avoid errors.
	LOG("Initializing ResourceManager");
	return true;
}

bool ResourceManager::CleanUp()
{
	return false;
}

uint ResourceManager::FindInAssets(const char* file_in_assets)
{
	uint id = 0;
	std::string MetaPath = file_in_assets;
	MetaPath.append(+".meta");
	
	ResourceType type;
	std::string Assets;
	std::string Library;
	TextureOptions* texOptions = new TextureOptions("");

	if (App->serializer->LoadMeta(MetaPath.c_str(), &id, &type, &Assets, &Library, texOptions))
	{
		if (App->file_system->CheckFile(Library.c_str()))
		{
			if (SearchForResource(id))
			{
				return id;
			}
			else
			{
				Resource* NewResource = nullptr;

				switch (type) {
				case ResourceType::MODEL: NewResource = (Resource*) new ResourceModel(id); break;
				case ResourceType::TEXTURE:
				{
					ResourceTexture* textureres = new ResourceTexture(id);
					textureres->flipXY = texOptions->flipXY;
					textureres->wrapping = texOptions->wrapping;
					textureres->filtering = texOptions->filtering;
					NewResource = (Resource*)textureres;
				}break;
					//	case ResourceType::MESH: ret = (Resource*) new ResourceMesh(uid); break;
				//Load Param from meta, in the case of textures?
				}

				if (NewResource != nullptr)
				{
					resources[id] = NewResource;
					NewResource->SetAssetPath(Assets.c_str());
					NewResource->SetLibraryPath(Library.c_str());
					std::string file, extension;
					App->file_system->SplitFilePath(Assets.c_str(), &file, &extension);
					file.append(extension);
					NewResource->name = file.c_str();
				}
				return id;

			}
		}
		else
		{
			LOG("Library file [%s] not found", file_in_assets);
			return 0;
		}

	}
	else
	{
		LOG("Meta [%s] not found", MetaPath.c_str());
		return 0;

	}

	delete texOptions;
	return 0;

}

void ResourceManager::FindInLibrary(const char* file_in_library, uint id)
{

	std::string MetaPath = file_in_library;


	ResourceType type = ReturnType(file_in_library);

	
		if (App->file_system->CheckFile(file_in_library))
		{
			if (SearchForResource(id))
			{
				LOG("Resource from [%s] found", file_in_library);
			}
			else
			{
				LOG("Library file found, creating new resource from [%s]", file_in_library);
				Resource* NewResource = nullptr;

				switch (type) {
				case ResourceType::MODEL: NewResource = (Resource*) new ResourceModel(id); break;
				case ResourceType::TEXTURE: NewResource = (Resource*) new ResourceTexture(id); break;
				case ResourceType::MESH: NewResource = (Resource*) new ResourceMesh(id); break;

				}

				if (NewResource != nullptr)
				{
					resources[id] = NewResource;

					NewResource->SetLibraryPath(file_in_library);

				}

			}
		}
		else
		{
			LOG("Library file [%s] not found", file_in_library);
		}


}
uint ResourceManager::ImportFile(const char* new_file_in_assets)
{
	uint ret = 0;

	ResourceType type = ReturnType(new_file_in_assets);
	if (type == ResourceType::UNKNOWN)
	{
		LOG("Failed to import resource, unknown type");
		return ret;
	}

	Resource* resource = CreateNewResource(new_file_in_assets, type);
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->file_system->Load(new_file_in_assets, &buffer);

	switch (resource->GetType())
	{
	case ResourceType::UNKNOWN:
		break;
	case ResourceType::MODEL:
	{
		LOG("importing model from %s", new_file_in_assets);
		App->FBX->LoadFBX(buffer, fileSize, (ResourceModel*)resource);
		break;
	}

	case ResourceType::TEXTURE:
	{
		MaterialImporter::Import(buffer, fileSize);
		std::string path;
		std::string file, extension;
		App->file_system->SplitFilePath(new_file_in_assets, &file, &extension);
		MaterialImporter::Save(&buffer, file.c_str(), &path);
		LOG("importing texture from %s", new_file_in_assets);
	}

	break;
	case ResourceType::MESH:
		break;
	}
	//load buffers from physfs
	//add info to resource
	//save resource

	return ret = resource->GetUID();
}

Resource* ResourceManager::RefreshResource(Resource* resource)
{

	ResourceType type = resource->GetType();
	if (type == ResourceType::UNKNOWN)
	{
		LOG("Failed to import resource, unknown type");
	}
	ImportOptions* options = nullptr;
	switch (type)
	{
	case ResourceType::UNKNOWN:
		break;
	case ResourceType::MODEL:
	{
		options = App->UI->CreateImportObject(resource->GetAssetFile(), importType::MODEL);
		break;
	}

	case ResourceType::TEXTURE:
	{
		options = App->UI->CreateImportObject(resource->GetAssetFile(), importType::TEXTURE);
		break;
	}
	case ResourceType::MESH:
	{
		break;
	}
	}
	
	options->reference = resource;

	return resource;
}

Resource* ResourceManager::ImportFileStep1(const char* new_file_in_assets)
{
	
	ResourceType type = ReturnType(new_file_in_assets);
	if (type == ResourceType::UNKNOWN)
	{
		LOG("Failed to import resource, unknown type");
		return nullptr;
	}
	ImportOptions* options;
	switch (type)
	{
	case ResourceType::UNKNOWN:
		break;
	case ResourceType::MODEL:
	{
		options = App->UI->CreateImportObject(new_file_in_assets, importType::MODEL);
		break;
	}

	case ResourceType::TEXTURE:
	{
		options = App->UI->CreateImportObject(new_file_in_assets, importType::TEXTURE);
		break;
	}
	case ResourceType::MESH:
	{
		break;
	}
	}
	Resource* resource = CreateNewResource(new_file_in_assets, type);
	options->reference = resource;
	
	return resource;
}

uint ResourceManager::ImportFileStep2(const char* new_file_in_assets, ImportOptions* options)
{
	uint ret = 0;

	ResourceType type = ReturnType(new_file_in_assets);
	if (type == ResourceType::UNKNOWN)
	{
		LOG("Failed to import resource, unknown type");
		return ret;
	}

	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->file_system->Load(new_file_in_assets, &buffer);

	switch (options->reference->GetType())
	{
	case ResourceType::UNKNOWN:
		break;
	case ResourceType::MODEL:
	{
		LOG("importing model from %s", new_file_in_assets);
		App->FBX->LoadFBX(buffer, fileSize, (ResourceModel*)options->reference, options);
		break;
	}

	case ResourceType::TEXTURE:
	{
		TextureOptions* textureOptions = (TextureOptions*)options;
		ResourceTexture* resourceTexture = (ResourceTexture*)textureOptions->reference;
		resourceTexture->filtering = textureOptions->filtering;
		resourceTexture->wrapping = textureOptions->wrapping;
		resourceTexture->flipXY = textureOptions->flipXY;

		MaterialImporter::Import(buffer, fileSize);
		std::string path;
		std::string file, extension;
		App->file_system->SplitFilePath(new_file_in_assets, &file, &extension);
		MaterialImporter::Save(&buffer, file.c_str(), &path);
		LOG("importing texture from %s", new_file_in_assets);


			JSON_Value* root_value = json_value_init_object();
			JSON_Object* root_object;
			root_object = json_value_get_object(root_value);
			App->serializer->AddFloat(root_object, "UID", resourceTexture->GetUID());
			App->serializer->AddFloat(root_object, "Last modified", App->file_system->GetDate(resourceTexture->GetAssetFile()));
			App->serializer->AddString(root_object, "Type", "Texture"); 
			App->serializer->AddFloat(root_object, "Filtering", textureOptions->filtering);
			App->serializer->AddFloat(root_object, "Wrapping", textureOptions->wrapping);
			App->serializer->AddFloat(root_object, "FlipXY", textureOptions->flipXY);

			App->serializer->AddString(root_object, "Asset Path", resourceTexture->GetAssetFile());
			App->serializer->AddString(root_object, "Library path", resourceTexture->GetLibraryFile());

			file = file + extension + ".meta";
			App->serializer->SaveValueAsFile(root_value, file.c_str());

	}

	break;
	case ResourceType::MESH:
		break;
	}
	//load buffers from physfs
	//add info to resource
	//save resource
	if (options->reference->isLoaded == true)
	{
		//we need to refresh
		options->reference->loadResource();
	}
	return ret = options->reference->GetUID();
}

ResourceType ResourceManager::ReturnType(const char* assetsFile)
{
	ResourceType ret = ResourceType::UNKNOWN;
	std::string file;
	std::string extension;

	App->file_system->SplitFilePath(assetsFile, &file, &extension);

	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == "fbx" || extension == "model")
	{
		ret = ResourceType::MODEL;
	}
	else if (extension == "png" || extension == "dds" || extension == "tga")
	{
		ret = ResourceType::TEXTURE;
	}
	else if (extension == "mesh")
	{
		ret = ResourceType::MESH;
	}


	return ret;
}
uint ResourceManager::GenerateNewUID()
{
	LCG();
	LCG rand;
	uint UID = rand.Int();
	return UID;
}
Resource* ResourceManager::CreateNewResource(const char* assetsFile, ResourceType type)
{
	Resource* ret = nullptr;

	

	uint uid = GenerateNewUID();
	
	switch (type) {
	case ResourceType::MODEL: ret = (Resource*) new ResourceModel(uid); break;
	case ResourceType::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
//	case ResourceType::MESH: ret = (Resource*) new ResourceMesh(uid); break;

	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetPath(assetsFile);
		ret->GenLibraryPath(ret);

		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object;
		root_object = json_value_get_object(root_value);
		App->serializer->AddFloat(root_object, "UID", ret->GetUID());
		App->serializer->AddFloat(root_object, "Last modified", App->file_system->GetDate(assetsFile));
		switch (type) {
		case ResourceType::MODEL: App->serializer->AddString(root_object, "Type", "3D Model"); break;
		case ResourceType::TEXTURE: App->serializer->AddString(root_object, "Type", "Texture"); break;
		}
		App->serializer->AddString(root_object, "Asset Path", ret->GetAssetFile());
		App->serializer->AddString(root_object, "Library path", ret->GetLibraryFile());
		std::string name;
		std::string extension;

		App->file_system->SplitFilePath(assetsFile, &name, &extension);
		name = name + extension + ".meta";
		App->serializer->SaveValueAsFile(root_value, name.c_str());

	}
	return ret;
}

Resource* ResourceManager::RequestResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		//it->second->referenceCount++;
		if (!it->second->isLoaded)
		{
			it->second->loadResource();
			it->second->isLoaded = true;
		}
		/*fileSize = App->file_system->Load(NewMeshResource->GetLibraryFile(), &buffer);
		MeshImporter::Load(buffer, fileSize, NewMesh);*/
		it->second->references++;
		return it->second;
	}
	else
		return nullptr;

}
bool ResourceManager::SearchForResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{


		return true;
	}
	else
		return false;

}


//-------------------------------------------------------------------------------

Resource::Resource(uint id)
{
	UID = id;
	references = 0;
	type = ResourceType::UNKNOWN;
	assetsFile = "";
	LibraryFile = "";
	isLoaded = false;
	node = App->UI->createAssetNode(this);
	name = "none";
}

Resource::~Resource()
{
	node->to_delete = true;
}

uint Resource::GetUID() const
{
	return UID;
}

const char* Resource::GetAssetFile() const
{
	return assetsFile.c_str();
}

const char* Resource::GetLibraryFile() const
{
	return LibraryFile.c_str();
}
ResourceType Resource::GetType() const
{
	return type;
}

void Resource::SetAssetPath(const char* AssetsPath)
{
	assetsFile = AssetsPath;
}

void Resource::SetLibraryPath(const char* LibraryPath)
{
	LibraryFile = LibraryPath;
}

void Resource::GenLibraryPath(Resource* resource)
{
	std::string file;
	std::string extension;
	App->file_system->SplitFilePath(resource->assetsFile.c_str(), &file, &extension);

	switch (type)
	{
	case ResourceType::UNKNOWN:
		break;
	case ResourceType::MODEL:
		LibraryFile = "Assets/library/" + file + "model";
		break;
	case ResourceType::TEXTURE:
		LibraryFile = "Assets/library/" + file + "dds";
		break;
	case ResourceType::MESH:
		break;
	}
}

void Resource::loadResource()
{
	LOG("Unespecified resource type for loading");
}

void Resource::unloadResource()
{
	LOG("Unespecified resource type for unloading");
}
//-------------------------------------------------------------------------------

ResourceModel::ResourceModel(uint id) : Resource(id)
{
	type = ResourceType::MODEL;
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	branch = json_value_init_array();
	leaves = json_value_get_array(branch);

	json_object_set_value(root_object, "Game Objects", branch);
}

ResourceModel::~ResourceModel()
{

}

//-------------------------------------------------------------------------------

ResourceMesh::ResourceMesh(uint id) : Resource(id)
{
	type = ResourceType::MESH;
}

ResourceMesh::~ResourceMesh()
{
	unloadResource();

}

void ResourceMesh::loadResource()
{
	char* buffer = nullptr;
	uint fileSize = 0;

	fileSize = App->file_system->Load(GetLibraryFile(), &buffer);
	MeshImporter::Load(buffer, fileSize, this);
}

void ResourceMesh::unloadResource()
{
	glDeleteBuffers(1, &id_index);
	glDeleteBuffers(1, &id_normals);
	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_tex);

	if (vertex != nullptr)
	{
		delete[] vertex;
		vertex = nullptr;
		id_vertex = 0;
	}
	if (index != nullptr)
	{
		delete[] index;
		index = nullptr;
		id_index = 0;
	}
	if (normals != nullptr)
	{
		delete[] normals;
		normals = nullptr;
		id_normals = 0;
	}

	if (texCoords != nullptr)
	{
		delete[] texCoords;
		texCoords = nullptr;
		id_tex = 0;
	}
	isLoaded = false;
}
//-------------------------------------------------------------------------------

ResourceTexture::ResourceTexture(uint id) : Resource(id)
{
	type = ResourceType::TEXTURE;
	filtering = false;
	wrapping = false;
	flipXY = false;
}

ResourceTexture::~ResourceTexture()
{
	unloadResource();
}

void ResourceTexture::loadResource()
{
	char* buffer = nullptr;
	uint fileSize = 0;

	fileSize = App->file_system->Load(GetLibraryFile(), &buffer);
	MaterialImporter::Load(buffer, fileSize, this);
}


void ResourceTexture::unloadResource()
{
	glDeleteTextures(1, &(GLuint)texbuffer);
	isLoaded = false;
	texbuffer = 0;
}