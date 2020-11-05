#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "Module.h"
#include "Globals.h"

class Serializer : public Module
{
public:
	Serializer(Application* app, bool start_enabled = true);

	~Serializer();

	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();

	double get_Number(const char* file, const char* name);
	void serialization_example();
public:


};
#endif