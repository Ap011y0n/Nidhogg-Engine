#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "Primitive.h"
#include "Globals.h"
#include "PhysBody3D.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#ifdef _DEBUG
	#pragma comment (lib, "SourceCode/Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "SourceCode/Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "SourceCode/Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "SourceCode/Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "SourceCode/Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "SourceCode/Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled), world(nullptr)
{
	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	//delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	debug_draw->setDebugMode(2);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);

	// Big rectangle as ground
	{
		btCollisionShape* colShape = new btBoxShape(btVector3(200.0f, 2.0f, 200.0f));

		mat4x4 glMatrix = IdentityMatrix;
		glMatrix.translate(0.f, -2.f, 0.f);
		btTransform startTransform;
		startTransform.setFromOpenGLMatrix(&glMatrix);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);

	for (int n = 0; n < world->getDispatcher()->getNumManifolds(); n++)
	{
		btPersistentManifold* manifold = world->getDispatcher()->getManifoldByIndexInternal(n);
		if (manifold->getNumContacts() > 0)
		{
			PhysBody3D* body1 = (PhysBody3D*)manifold->getBody0()->getUserPointer();
			PhysBody3D* body2 = (PhysBody3D*)manifold->getBody1()->getUserPointer();

			/*if (body1 != nullptr && body2 != nullptr)
			{
				for (uint n = 0; n < body1->collision_listeners.Count(); n++)
				{
					body1->collision_listeners[n]->OnCollision(body1, body2);
				}

				for (uint n = 0; n < body2->collision_listeners.Count(); n++)
				{
					body2->collision_listeners[n]->OnCollision(body2, body1);
				}
			}*/
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	world->debugDrawWorld();

	/*bool draw = true;
	if (draw)
	{
		glDisable(GL_LIGHTING);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		world->debugDrawWorld();
		glEnable(GL_LIGHTING);
	}*/

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------


// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}
	for (int i = 0; i < P2PConstraints.size(); i++)
	{
		world->removeConstraint(P2PConstraints[i]);
		delete P2PConstraints[i];
	}
	
	for (int i = 0; i < HingeConstraints.size(); i++)
	{
		world->removeConstraint(HingeConstraints[i]);
		delete HingeConstraints[i];
	}
	for (int i = 0; i < SliderConstraints.size(); i++)
	{
		world->removeConstraint(SliderConstraints[i]);
		delete SliderConstraints[i];
	}
	
	
	delete world;

	return true;
}
//
//PhysBody3D * ModulePhysics3D::RayCast(const vec3 & Origin, const vec3 & Direction, vec3& HitPoint)
//{
//	//TODO: NEW CODE
//	//A snippet of new code that may be useful for you. Nothing to do here really
//	vec3 Dir = normalize(Direction);
//
//	btVector3 Start = btVector3(Origin.x, Origin.y, Origin.z);
//	btVector3 End = btVector3(Origin.x + Dir.x * 1000.f, Origin.y + Dir.y * 1000.f, Origin.z + Dir.z * 1000.f);
//
//	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
//
//	// Perform raycast
//	world->rayTest(Start, End, RayCallback);
//	if (RayCallback.hasHit()) {
//
//		HitPoint = vec3(RayCallback.m_hitPointWorld.x(), RayCallback.m_hitPointWorld.y(), RayCallback.m_hitPointWorld.z());
//		return (PhysBody3D*)RayCallback.m_collisionObject->getUserPointer();
//	}
//	return nullptr;
//}

void ModulePhysics3D::AddBodyToWorld(btRigidBody * body)
{
	world->addRigidBody(body);
}

void ModulePhysics3D::RemoveBodyFromWorld(btRigidBody * body)
{
	world->removeRigidBody(body);
}


//btPoint2PointConstraint* ModulePhysics3D::AddConstraintP2P(const Primitive& bodyA, const Primitive& bodyB, btVector3& pivotInA, btVector3& pivotInB) {
//	btPoint2PointConstraint* constraint = new btPoint2PointConstraint(*bodyA.body.GetBody(), *bodyB.body.GetBody(), pivotInA, pivotInB);
//	world->addConstraint(constraint);
//	P2PConstraints.push_back(constraint);
//	return constraint;
//}
//
//btHingeConstraint* ModulePhysics3D::AddConstraintHinge(const Primitive& bodyA, const Primitive& bodyB, btVector3& pivotInA, btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB) {
//	btHingeConstraint* constraint = new btHingeConstraint(*bodyA.body.GetBody(), *bodyB.body.GetBody(), pivotInA, pivotInB, axisInA, axisInB);
//	world->addConstraint(constraint);
//	HingeConstraints.push_back(constraint);
//	return constraint;
//}
//
//btSliderConstraint* ModulePhysics3D::AddConstraintSlider(const Primitive& bodyA, const Primitive& bodyB, btTransform& frameinA, btTransform& frameinB, bool linearreference) {
//	btSliderConstraint* constraint = new btSliderConstraint(*bodyA.body.GetBody(), *bodyB.body.GetBody(),frameinA, frameinB, linearreference);
//	constraint->setLowerLinLimit(0.f);
//	constraint->setUpperLinLimit(0.4f);
//	constraint->setLowerAngLimit(-0.2f);
//	constraint->setUpperAngLimit(0.2f);
//	
//	world->addConstraint(constraint);
//	SliderConstraints.push_back(constraint);
//
//	return constraint;
//}

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}