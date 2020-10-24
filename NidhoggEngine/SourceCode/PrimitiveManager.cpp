#include "Globals.h"
#include "Application.h"
#include "PrimitiveManager.h"
#include "Primitive.h"
#include "ModuleWindow.h"
#include "glew/include/glew.h"



PrimitiveManager::PrimitiveManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

PrimitiveManager::~PrimitiveManager()
{}

// Load assets
bool PrimitiveManager::Start()
{
	bool ret = true;
	depth_bool = false;
	cullface_bool = false;	
	LIGHTING_bool = false;
	Color_bool = false;
	return ret;
}

// Load assets
bool PrimitiveManager::CleanUp()
{
	LOG("Unloading Primitive Manager");
	for (int i = 0; i < prim_list.size(); i++) {

		delete prim_list[i];

	}
	return true;
}

// Update: draw background
update_status PrimitiveManager::PostUpdate(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		for (int i = 0; i < prim_list.size(); i++) {
			
			prim_list[i]->wire = !prim_list[i]->wire;
		}
	}
	MenuOptions();

	for (int i = 0; i < prim_list.size(); i++) {
		
		prim_list[i]->Render();
		
	}
	
	return UPDATE_CONTINUE;
}

Cube* PrimitiveManager::CreateCube(vec3 size, vec3 pos)
{
	Cube* cube = new Cube(size.x, size.y, size.z);
	cube->SetPos(pos.x, pos.y, pos.z);
	prim_list.push_back((Primitive*)cube);
	return cube;
}

Pyramid* PrimitiveManager::CreatePyramid(vec3 size, vec3 pos)
{
	Pyramid* pyramid = new Pyramid(size.x, size.y, size.z);
	pyramid->SetPos(pos.x, pos.y, pos.z);
	prim_list.push_back((Primitive*)pyramid);
	return pyramid;
}


PrimSphere* PrimitiveManager::CreateSphere(float radius, unsigned int rings, unsigned int sectors, vec3 pos)
{
	PrimSphere* sphere = new PrimSphere(radius, rings, sectors);
	sphere->SetPos(pos.x, pos.y, pos.z);
	prim_list.push_back((Primitive*)sphere);
	return sphere;
}

PrimCylinder* PrimitiveManager::CreateCylinder(float radius, float height, int sides, vec3 pos)
{
	PrimCylinder* cylinder = new PrimCylinder(radius, height, sides);
	cylinder->SetPos(pos.x, pos.y, pos.z);
	prim_list.push_back((Primitive*)cylinder);
	return cylinder;
}

void PrimitiveManager::CreateLine(vec3 origin, vec3 dest)
{
	Line* line = new Line(origin.x, origin.y, origin.z, dest.x, dest.y, dest.z);
	prim_list.push_back((Primitive*)line);
}
void PrimitiveManager::CreatePlane(vec4 coords, bool axis)
{
	PrimPlane* plane = new PrimPlane(coords.x, coords.y, coords.z, coords.w);
	plane->axis = true;
	prim_list.push_back((Primitive*)plane);
}

void PrimitiveManager::MenuOptions() 
{
	if (depth_bool)
	{
		glDisable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
	}

	if (cullface_bool)
	{
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	if (texture2D_bool)
	{
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
	}

	if (LIGHTING_bool) {
		glDisable(GL_LIGHTING);
	}
	else
	{
		glEnable(GL_LIGHTING);
	}

	if (Color_bool)
	{
		glDisable(GL_COLOR_MATERIAL);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
	}



}