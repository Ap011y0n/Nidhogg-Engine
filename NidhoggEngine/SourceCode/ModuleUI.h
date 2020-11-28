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

	bool CleanUp();

	void AboutMenu(bool window);
	void StoreLog(const char* message);
	void ShowAppinDockSpace(bool* p_open = NULL);

	void Configuration(bool config);
	void PlotGraph();
	void HierarchyWin(); //hierarchy window
	void AssetsTree(); //hierarchy window
	void InspectorWin(); //gameobjects list
	void TimeMangmentWin();
	void ShowExampleAppLayout(/*bool* p_open*/);
	void Change_Window_size(Vec2 newSize);

	void GameObjectHierarchyTree(GameObject* node, int id);
	void Change_Visibility_BoundingBoxes(GameObject* node,bool visibility);
	void AssetsHierarchyTree(AssetNode* node);

	void DeactivateGameObjects(GameObject* father);
	void DeactivateAssets();

	void GameObjectInspector(GameObject* obj);
	void ChangeParent(GameObject* obj, GameObject* nextOwner);
	void SelectGameObject(GameObject* node);
	void GuizmoUI();
	void ControlsGuizmo();
	void RightClick_Inspector_Menu();
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
	 bool Console_open;

	vector <float> fpsecond;
	bool show_Configuration;
	int max_fps = 61;
	int i = 0;
	int e = 0;
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

private:
	int c1;
	vector<string> consoleOutput;
	float width = 1324;
	float height = 768;
};

