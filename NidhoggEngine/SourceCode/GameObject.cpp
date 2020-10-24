#include "GameObject.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#include "MathGeoLib/include/MathGeoLib.h"

//*************************		GameObject
GameObject::GameObject()
{
	active = true;
	Name = "NewGameObject";
}

GameObject::GameObject(const char* name)
{
	Name = name;
	active = true;

}

GameObject::~GameObject()
{}

bool GameObject::Update(float dt)
{
	bool ret = true;

	for (int i = 0; i < Components.size(); i++)
	{
		Components[i]->Update(dt);
	}

	return ret;
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* newComponent = nullptr;
	switch (type)
	{
	case ComponentType::NONE: break;
	case ComponentType::MESH: { ComponentMesh* mesh = new ComponentMesh(this); newComponent = mesh; break; }
	case ComponentType::MATERIAL: { ComponentMaterial* material = new ComponentMaterial(this); newComponent = material; break; }
	case ComponentType::TRANSFORM: { ComponentTransform* transform = new ComponentTransform(this); newComponent = transform; break; }

	}
	Components.push_back(newComponent);

	return newComponent;
}

//*************************		Component
Component::Component()
{
	owner = nullptr;
	active = false;
	type = ComponentType::NONE;
}

Component::~Component()
{
}

void Component::Enable()
{
	if (!active)
		active = true;
}

bool Component::Update(float dt)
{
	bool ret = true;
	return ret;
}

void Component::Disable()
{
	if (active)
		active = false;
}

//*************************		ComponentMesh

ComponentMesh::ComponentMesh(GameObject* ObjectOwner) : Component()
{
	id_index = 0;
	num_index = 0;
	index = nullptr;

	id_normals = 0;
	num_normals = 0;
	normals = nullptr;

	id_vertex = 0;
	num_vertex = 0;
	vertex = nullptr;

	id_tex = 0;
	num_tex = 0;
	texCoords = nullptr;

	type = ComponentType::MESH;
	active = true;
	owner = ObjectOwner;
}

ComponentMesh::~ComponentMesh()
{
}

bool ComponentMesh::Update(float dt)
{
	bool ret = true;
	ComponentMaterial* material = nullptr; 
	ComponentTransform* transform = nullptr;

	for (int i = 0; i < owner->Components.size(); i++)
	{
		if (owner->Components[i]->type == ComponentType::MATERIAL)
		{
			material = (ComponentMaterial*)owner->Components[i];
		}
		if (owner->Components[i]->type == ComponentType::TRANSFORM)
		{
			transform = (ComponentTransform*)owner->Components[i];
		}
	}

		glPushMatrix();
		if (transform != nullptr)
		{
			glMultMatrixf(transform->transform.M);
		}
		else
		{
			mat4x4 mat;
			glMultMatrixf(mat.M);
		}
	
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);


		glBindBuffer(GL_ARRAY_BUFFER, id_tex);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	
				if (material != nullptr && material->hastexture)
				{
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_CULL_FACE);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, material->texbuffer);
				}
			
		
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);

		glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();


	return ret;
}

//*************************		ComponentMaterial

ComponentMaterial::ComponentMaterial(GameObject* ObjectOwner) : Component()
{
	type = ComponentType::MATERIAL;
	active = true;
	owner = ObjectOwner;
	texbuffer = 0;
	hastexture = false;
}

ComponentMaterial::~ComponentMaterial()
{
}

bool ComponentMaterial::Update(float dt)
{
	bool ret = true;

	return ret;
}


//*************************		ComponentTransform

ComponentTransform::ComponentTransform(GameObject* ObjectOwner) : Component()
{
	type = ComponentType::TRANSFORM;
	active = true;
	owner = ObjectOwner;

	float3 pos(0, 0, 0);
	float3 scale(0, 0, 0);
	Quat rot(0, 0, 0, 0);

}

ComponentTransform::~ComponentTransform()
{
}

bool ComponentTransform::Update(float dt)
{
	bool ret = true;
	UpdatePos(pos.x, pos.y, pos.z);
	//UpdateRotation(pos.x, pos.y, pos.z);
	UpdateScale(scale.x, scale.y, scale.z);
	return ret;
}
void ComponentTransform::UpdatePos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

void ComponentTransform::UpdateRotation(float angle, const vec3& u)
{
	transform.rotate(angle, u);
}

void ComponentTransform::UpdateScale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

void ComponentTransform::SetPos(float x, float y, float z)
{
	pos.Set(x, y, z);
}	
void ComponentTransform::SetRotation(float x, float y, float z, float w)
{
	rot.Set(x, y, z, w);

}

void ComponentTransform::Scale(float x, float y, float z)
{
	scale.Set(x, y, z);
}

