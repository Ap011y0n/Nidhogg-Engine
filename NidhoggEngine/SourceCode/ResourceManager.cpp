#include "Application.h"
#include "ResourceManager.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "parson/parson.h"

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

uint ResourceManager::Find(const char* file_in_assets)
{
	uint id = 0;
	std::string MetaPath = file_in_assets;
	MetaPath.append(+".meta");
	
	ResourceType type;
	std::string Assets;
	std::string Library;
	if (App->serializer->LoadMeta(MetaPath.c_str(), &id, &type, &Assets, &Library))
	{
		LOG("Meta found");
		if (App->file_system->CheckFile(Library.c_str()))
		{
			LOG("Library file found");
			if (SearchForResource(id))
			{
				LOG("Resource found in resources map");
				return id;
			}
			else
			{
				LOG("Resource not found in resoruces, you should add this resource");

				//Resource* NewResource = nullptr;

				//switch (type) {
				//case ResourceType::MODEL: NewResource = (Resource*) new ResourceModel(id); break;
				//case ResourceType::TEXTURE: NewResource = (Resource*) new ResourceTexture(id); break;
				//	//	case ResourceType::MESH: ret = (Resource*) new ResourceMesh(uid); break;

				//}

				//if (NewResource != nullptr)
				//{
				//	resources[id] = NewResource;
				//	NewResource->SetAssetPath(Assets.c_str());
				//	NewResource->SetLibraryPath(Library.c_str());

				//}
				return 0;

			}
		}
		else
		{
			LOG("Library file not found");
			return 0;
		}

	}
	else
	{
		LOG("Meta not found");
		return 0;

	}

	return 0;

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
		LOG("importing model from %s", new_file_in_assets);
		App->FBX->LoadFBX(buffer, fileSize, (ResourceModel*)resource);
		break;
	case ResourceType::TEXTURE:
		LOG("importing texture from %s", new_file_in_assets);
		break;
	case ResourceType::MESH:
		break;
	}
	//load buffers from physfs
	//add info to resource
	//save resource

	return ret = resource->GetUID();
}

ResourceType ResourceManager::ReturnType(const char* assetsFile)
{
	ResourceType ret = ResourceType::UNKNOWN;
	std::string file;
	std::string extension;

	App->file_system->SplitFilePath(assetsFile, &file, &extension);

	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == "fbx")
	{
		ret = ResourceType::MODEL;
	}
	if (extension == "png" || extension == "dds" || extension == ".tga")
	{
		ret = ResourceType::TEXTURE;
	}
	return ret;
}
uint ResourceManager::GenerateNewUID()
{
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
	type = ResourceType::UNKNOWN;
	assetsFile = "";
	LibraryFile = "";
	isLoaded = false;
}

Resource::~Resource()
{

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
	LOG("Unespecified resource type");
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

}

void ResourceMesh::loadResource()
{
	char* buffer = nullptr;
	uint fileSize = 0;

	fileSize = App->file_system->Load(GetLibraryFile(), &buffer);
	MeshImporter::Load(buffer, fileSize, this);
}
//-------------------------------------------------------------------------------

ResourceTexture::ResourceTexture(uint id) : Resource(id)
{
	type = ResourceType::TEXTURE;
}

ResourceTexture::~ResourceTexture()
{

}

void ResourceTexture::loadResource()
{
	char* buffer = nullptr;
	uint fileSize = 0;

	fileSize = App->file_system->Load(GetLibraryFile(), &buffer);
	MaterialImporter::Load(buffer, fileSize, this);
}