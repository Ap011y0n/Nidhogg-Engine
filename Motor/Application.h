#ifndef _APPLICATION_H
#define _APPLICATION_H



#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "PrimitiveManager.h"
#include "FBXloader.h"
#include "FileSystem.h"

#include <vector>

using namespace std;


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleUI* UI;
	PrimitiveManager* PrimManager;
	FBXloader* FBX;
	FileSystem* file_system;
private:

	Timer	ms_timer;
	float	dt;
	vector<Module*> list_modules;

	int fps = 60;
	int cap = -1;
	int frame_count = 0;
	int last_sec_frame_count = 0;
	Timer last_sec_frame_time;
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	int GetFPS();
	void Maxfps(int max_fps);
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif // !_APPLICATION_H
