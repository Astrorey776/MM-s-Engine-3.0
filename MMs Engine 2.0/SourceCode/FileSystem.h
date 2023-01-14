#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Module.h"



class UiFile
{
public:
	UiFile(const char* filename, const char* extension, const char* path);
	~UiFile();
	std::string file;
	std::string fullpath;
	std::string extension;
};

enum class FileType
{
	UNKNOWN,
	FBX,
	IMAGE,
	MESH
};
class FileSystem : public Module
{
public:

	FileSystem(Application* app, bool start_enabled = true);
	~FileSystem();

	void SplitFilePath(const char* full_path, std::string* file, std::string* extension) const;
	FileType SetFileType(std::string file);
	const char* GetWriteDir();
	// Called before render is available
	std::string substractPrefix(std::string dir);
	bool Init();

	// Called before quitting
	bool CleanUp();

	// Return the bytes of a PhysFS filehandle
	uint Load(const char* path, char** buffer) const;
	bool CheckFile(const char* path) const;
	bool RemoveFile(const char* path);
	unsigned int Save(const char* file, const char* buffer, unsigned int size, bool append) const;
	void checkDirectoryFiles(const char* currentDirectory, vector<UiFile*>* fileVec);
	void checkDirectoryFolders(const char* currentDirectory, FolderNode* node);
	void importAssetsFiles();
	void RefreshAssets();
	int GetDate(const char* path);

};

#endif // __ASSETSMANAGER_H__


