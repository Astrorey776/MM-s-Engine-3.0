#include "Application.h"
#include "FileSystem.h"

#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"

#pragma comment( lib, "SourceCode/PhysFS/libx86/physfs.lib" )
#include "PhysFS/include/physfs.h"

FileSystem::FileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	// TODO 0 (Solved): Open the following link and check it: https://icculus.org/physfs/docs/html/physfs_8h.html
	// TODO 1 (Solved): You must initialize and de-initialize the PhysFS API.

	// Initialize the PhysicsFS library
	// This must be called before any other PhysicsFS function
	// This should be called prior to any attempts to change your process's current working directory
	PHYSFS_init(nullptr);

	// We only need this when compiling in debug. In Release we don't need it.
	PHYSFS_mount(".", nullptr, 1);
}


FileSystem::~FileSystem()
{
	// Deinitialize the PhysicsFS library.
	// This closes any files opened via PhysicsFS, blanks the search/write paths, frees memory, and invalidates all of your file handles.
	// NOTE: This call can FAIL if there's a file open for writing that refuses to close
	PHYSFS_deinit();
}

bool FileSystem::Init()
{
	// Determine if the PhysicsFS library is initialized, we can check it for avoid errors.
	if (PHYSFS_isInit())
	{
		LOG("Asset Manager is succefully loaded");
	}
	else
		LOG("Failed loading Asset Manager");

	// TODO 2 (Solved): Mount the desired ZIP with PhysFS.

	// Add an archive or directory to the search path.
	// If this is a duplicate, the entry is not added again, even though the function succeeds.
	// When you mount an archive, it is added to a virtual file system...
	// all files in all of the archives are interpolated into a single hierachical file tree.
	if (PHYSFS_mount("Assets", nullptr, 1) == 0)
	{
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	PHYSFS_setWriteDir("Assets/");
//	PHYSFS_setWriteDir("Assets/library");
	return true;
}

const char* FileSystem::GetWriteDir()
{
	return	PHYSFS_getWriteDir();
}

bool FileSystem::CleanUp()
{
	return false;
}


void FileSystem::SplitFilePath(const char* full_path, std::string* file, std::string* extension) const
{
	if (full_path != nullptr)
	{
		std::string full("/");
		full += full_path;
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");


		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

std::string FileSystem::substractPrefix(std::string dir)
{
	size_t pos_separator = dir.find_first_of("\\/") + 1;

	std::string ret = dir.substr(pos_separator, dir.back());
	return ret;
}

FileType FileSystem::SetFileType(std::string extension)
{
	FileType ret = FileType::UNKNOWN;
	transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == "fbx")
	{
		ret = FileType::FBX;
	}

	if (extension == "png" || extension == "dds" || extension == "tga")
	{
		ret = FileType::IMAGE;
	}
	if (extension == "mesh")
	{
		ret = FileType::MESH;
	}

	return ret;
}

bool FileSystem::CheckFile(const char* path) const
{
	PHYSFS_file* file = PHYSFS_openRead(path);
	if (file == NULL)
		return false;
	PHYSFS_close(file);
	return true;
}

bool FileSystem::RemoveFile(const char* path)
{
	bool ret = false;
	/*if (CheckFile(path))
	{*/
		if (PHYSFS_delete(path) != 0)
		{
			LOG("Deleting [%s]", path);
			ret = true;
		}
		else
		{
			LOG("error while trying to delete [%s]: %s", path, PHYSFS_getLastError());
		}
//	}
	
	/*else
	{
		LOG("couldn't delete file %s, path not found", path);
	}*/
	return ret;
}
uint FileSystem::Load(const char* path, char** buffer) const
{
	uint ret = 0;
	LOG("Loading file from %s", path);
	// TODO 3 (Solved): You want to return the number of bytes it has read from the file that we passed to this function. 
	// Maybe you want to search readBytes in the documentation, and investigate from there how to build the function.

	// The reading offset is set to the first byte of the file.
	// Returns a filehandle on success that we will need for the PHYSFS_fileLength
	PHYSFS_file* file = PHYSFS_openRead(path);

	// Check for end-of-file state on a PhysicsFS filehandle.
	if (file != nullptr)
	{
		// Get total length of a file in bytes
		uint lenght = PHYSFS_fileLength(file); 
		*buffer = new char[lenght]; 

		// Read data from a PhysicsFS firehandle. Returns a number of bytes read.
		uint bytes = PHYSFS_readBytes(file, *buffer, lenght);

		if (bytes != lenght) 
		{
			LOG("%s" , path, "ERROR: %s" , PHYSFS_getLastError());
			{                              
				if (buffer != NULL)              
				{                            
					delete[] buffer;                
					buffer = NULL;                    
				}                            
					
			}
		}
		else
			ret = bytes; 
	}
	else
		LOG("ERROR: %s", PHYSFS_getLastError());


	// Close a PhysicsFS firehandle
	PHYSFS_close(file);

	return ret;
}

// Save a whole buffer to disk
unsigned int FileSystem::Save(const char* file, const char* buffer, unsigned int size, bool append) const
{
	unsigned int ret = 0;


	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint64 written = PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError());

		}
		else
		{
			if (append == true)
			{
				LOG("Added %u data to [%s/%s]", size, PHYSFS_getWriteDir(), file);
			}
			else if (overwrite == true)
			{
				LOG("File [%s/%s] overwritten with %u bytes", PHYSFS_getWriteDir(), file, size);
			}
			else
			{
				LOG("New file created [%s/%s] of %u bytes", PHYSFS_getWriteDir(), file, size);
			}

			ret = (uint)written;

		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}



void FileSystem::checkDirectoryFiles(const char* currentDirectory, vector<UiFile*>* fileVec)
{
	fileVec->clear();
	if (PHYSFS_exists(currentDirectory) != 0)
	{
		char** Assets_List = PHYSFS_enumerateFiles(currentDirectory);
		char** i;

		for (i = Assets_List; *i != nullptr; i++)
		{
			std::string extension;
			std::string file;

			SplitFilePath(*i, &file, &extension);
			std::string fullpath = currentDirectory;
			fullpath += "/" + file + extension;

			UiFile* filefound = new UiFile(file.c_str(), extension.c_str(), fullpath.c_str());
			fileVec->push_back(filefound);
		}

	}
}


void FileSystem::checkDirectoryFolders(const char* currentDirectory, FolderNode *node)
{
	if (PHYSFS_exists(currentDirectory) != 0)
	{
		char** Assets_List = PHYSFS_enumerateFiles(currentDirectory);
		char** i;

		for (i = Assets_List; *i != nullptr; i++)
		{
			std::string extension;
			std::string file;

			SplitFilePath(*i, &file, &extension);
			std::string fullpath = currentDirectory;
			fullpath += "/" + file + extension;

			if (extension == "") 
			{
				FolderNode* NewFolder = new FolderNode(file.c_str(), node);
				NewFolder->parent = node->parent;
				checkDirectoryFolders(fullpath.c_str(), NewFolder);
			}
		}

	}
}

void FileSystem::importAssetsFiles()
{
	if (PHYSFS_exists("Assets/") != 0)
	{
		char** Assets_List = PHYSFS_enumerateFiles("Assets/");
		char** i;

		for (i = Assets_List; *i != nullptr; i++)
		{
			std::string extension;
			std::string file;

			SplitFilePath(*i, &file, &extension);
			std::string fullpath = "Assets/";
			fullpath += file + extension;
			uint UID = 0;
			switch (SetFileType(extension))
			{
			case FileType::FBX:
				UID = App->ResManager->FindInAssets(fullpath.c_str());
				if (UID == 0)
				{
					App->ResManager->ImportFileStep1(fullpath.c_str());
				}
				break;
			case FileType::IMAGE:
				UID = App->ResManager->FindInAssets(fullpath.c_str());
				if (UID == 0)
				{
					App->ResManager->ImportFileStep1(fullpath.c_str());
				}
				break;
			}
			
		}

	}
}

void FileSystem::RefreshAssets()
{
	if (PHYSFS_exists("Assets/") != 0)
	{
		char** Assets_List = PHYSFS_enumerateFiles("Assets/");
		char** i;

		for (i = Assets_List; *i != nullptr; i++)
		{
			std::string extension;
			std::string file;

			SplitFilePath(*i, &file, &extension);
			std::string fullpath = "Assets/";
			fullpath += file + extension;
			uint UID = 0;
			FileType type = SetFileType(extension);
			if (type == FileType::FBX || type == FileType::IMAGE)
			{
				UID = App->ResManager->FindInAssets(fullpath.c_str());
				if (UID == 0)
				{
					App->ResManager->ImportFileStep1(fullpath.c_str());
				}
				else
				{

					uint id = 0;
					std::string MetaPath = fullpath;
					MetaPath.append(+".meta");

					ResourceType type;
					std::string Assets;
					std::string Library;
					uint timestamp1, timestamp2;
					App->serializer->LoadMeta(MetaPath.c_str(), &id, &type, &Assets, &Library, nullptr, &timestamp1);
					timestamp2 = GetDate(fullpath.c_str());
					if (timestamp1 != timestamp2)
					{
						LOG("needs update");
						Resource* NewResource = App->ResManager->RequestResource(UID);
						App->ResManager->RefreshResource(NewResource);
					}
				}
			}
		

		}

	}
}
int FileSystem::GetDate(const char* path)
{
	return PHYSFS_getLastModTime(path);
}


UiFile::UiFile(const char* name, const char* ext, const char* path)
{
	extension = ext;
	file = name;
	fullpath = path;
}