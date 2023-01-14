#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "Primitive.h"
#include "GameObject.h"
#include "Globals.h"
#include "PhysBody3D.h"
#include "Time.h"
#include "ModuleCamera3D.h"
#include "PhysVehicle3D.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/include/MathGeoLib.h"

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
	debug_draw->setDebugMode(1);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);
	gravity = -10.0f;
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	// Big rectangle as ground
	/*{
		btCollisionShape* colShape = new btBoxShape(btVector3(200.0f, 2.0f, 200.0f));

		mat4x4 glMatrix = IdentityMatrix;
		glMatrix.translate(0.f, -2.f, 0.f);
		btTransform startTransform;
		startTransform.setFromOpenGLMatrix(&glMatrix);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}*/

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(Time::delta_time_fisics, 15);

	for (int n = 0; n < world->getDispatcher()->getNumManifolds(); n++)
	{
		btPersistentManifold* manifold = world->getDispatcher()->getManifoldByIndexInternal(n);
		if (manifold->getNumContacts() > 0)
		{
			PhysBody3D* body1 = (PhysBody3D*)manifold->getBody0()->getUserPointer();
			PhysBody3D* body2 = (PhysBody3D*)manifold->getBody1()->getUserPointer();

			if (body1 != nullptr && body2 != nullptr)
			{
				for (uint n = 0; n < body1->collision_listeners.size(); n++)
				{
					body1->collision_listeners[n]->OnCollision(body1, body2);
				}

				for (uint n = 0; n < body2->collision_listeners.size(); n++)
				{
					body2->collision_listeners[n]->OnCollision(body2, body1);
				}
			}
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

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) 
	{
		float radius = 1;
		int rings = 40;
		vec3 pos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
		Primitive* sphere1 = App->PrimManager->CreateSphere(radius, rings, 116 , pos);
		float force = 900.0f;
		sphere1->body.Push({ -(App->camera->Z.x * force), -(App->camera->Z.y * force), -(App->camera->Z.z * force) });
	}

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
	
	delete vehicle_raycaster;
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

// ---------------------------------------------------------
PhysVehicle3D* ModulePhysics3D::AddVehicle(const VehicleInfo& info)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.push_back(comShape);

	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z * 0.5f));
	shapes.push_back(colShape);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));

	comShape->addChildShape(trans, colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}
	// ---------------------
//	PhysVehicle3D* pvehicle = nullptr;
	PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
	world->addVehicle(vehicle);
	//vehicles.push_back(pvehicle);

	return pvehicle;
}


btPoint2PointConstraint* ModulePhysics3D::AddConstraintP2P(const Primitive& bodyA, const Primitive& bodyB, btVector3& pivotInA, btVector3& pivotInB) {
	
	btRigidBody body1 = *bodyA.body.GetBody();
	btRigidBody body2 = *bodyB.body.GetBody();
	btPoint2PointConstraint* constraint = new btPoint2PointConstraint(*bodyA.body.GetBody(), *bodyB.body.GetBody(), pivotInA, pivotInB);
	world->addConstraint(constraint);
	P2PConstraints.push_back(constraint);
	return constraint;
}

btPoint2PointConstraint* ModulePhysics3D::AddConstraintP2P(GameObject* bodyA, GameObject* bodyB, btVector3& dist) {
	if (bodyA && bodyB)
	{
		Collider* colliderA = (Collider*)bodyA->GetComponent(ComponentType::COLLIDER);
		Collider* colliderB = (Collider*)bodyB->GetComponent(ComponentType::COLLIDER);


		if (colliderA && colliderB && colliderA != colliderB)
		{
			Constraint* newConstraint = new Constraint(colliderA, colliderB, ConstraintType::DISTANCE);
			App->scene_intro->constraints.push_back(newConstraint);
			newConstraint->distance.Set(dist.getX(), dist.getY(), dist.getZ());
			btVector3 pivotinA, pivotinB;
			SetPivots(dist, pivotinA, pivotinB, colliderA, colliderB);

			btRigidBody* body1 = colliderA->body.GetBody();
			btRigidBody* body2 = colliderB->body.GetBody();
			btPoint2PointConstraint* constraint = new btPoint2PointConstraint(*body1, *body2, pivotinA, pivotinB);
			world->addConstraint(constraint);
			P2PConstraints.push_back(constraint);
			newConstraint->ConstraintPointer = constraint;
			return constraint;

		}
	}
	return nullptr;
}

btHingeConstraint* ModulePhysics3D::AddConstraintHinge(const Primitive& bodyA, const Primitive& bodyB, btVector3& pivotInA, btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB) {
	btHingeConstraint* constraint = new btHingeConstraint(*bodyA.body.GetBody(), *bodyB.body.GetBody(), pivotInA, pivotInB, axisInA, axisInB);
	world->addConstraint(constraint);
	HingeConstraints.push_back(constraint);
	return constraint;
}

btHingeConstraint* ModulePhysics3D::AddConstraintHinge(GameObject* bodyA, GameObject* bodyB, btVector3& dist, btVector3& axisInA, btVector3& axisInB) {
	if (bodyA && bodyB)
	{
		Collider* colliderA = (Collider*)bodyA->GetComponent(ComponentType::COLLIDER);
		Collider* colliderB = (Collider*)bodyB->GetComponent(ComponentType::COLLIDER);

		if (colliderA && colliderB && colliderA != colliderB)
		{
			Constraint* newConstraint = new Constraint(colliderA, colliderB, ConstraintType::HINGE);
			App->scene_intro->constraints.push_back(newConstraint);
			newConstraint->distance.Set(dist.getX(), dist.getY(), dist.getZ());
			newConstraint->axis1.Set(axisInA.getX(), axisInA.getY(), axisInA.getZ());
			newConstraint->axis2.Set(axisInB.getX(), axisInB.getY(), axisInB.getZ());
			newConstraint->motor = false;
			newConstraint->velocity = 5.f;
			newConstraint->maxspeed = 100.f;
			btVector3 pivotinA, pivotinB;
			SetPivots(dist, pivotinA, pivotinB, colliderA, colliderB);

			btHingeConstraint* constraint = new btHingeConstraint(*colliderA->body.GetBody(), *colliderB->body.GetBody(), pivotinA, pivotinB, axisInA, axisInB);
			world->addConstraint(constraint);
			HingeConstraints.push_back(constraint);
			newConstraint->ConstraintPointer = constraint;

			return constraint;

		}
	}
	return nullptr;
}

btSliderConstraint* ModulePhysics3D::AddConstraintSlider(const Primitive& bodyA, const Primitive& bodyB, btTransform& frameinA, btTransform& frameinB, bool linearreference) {
	btSliderConstraint* constraint = new btSliderConstraint(*bodyA.body.GetBody(), *bodyB.body.GetBody(),frameinA, frameinB, linearreference);
	constraint->setLowerLinLimit(0.f);
	constraint->setUpperLinLimit(0.4f);
	constraint->setLowerAngLimit(-0.2f);
	constraint->setUpperAngLimit(0.2f);
	
	world->addConstraint(constraint);
	SliderConstraints.push_back(constraint);

	return constraint;
}

btSliderConstraint* ModulePhysics3D::AddConstraintSlider(GameObject* bodyA, GameObject* bodyB, btVector3& dist, bool linearreference) {
	if (bodyA && bodyB)
	{
		Collider* colliderA = (Collider*)bodyA->GetComponent(ComponentType::COLLIDER);
		Collider* colliderB = (Collider*)bodyB->GetComponent(ComponentType::COLLIDER);
		if (colliderA && colliderB && colliderA != colliderB)
		{
			Constraint* newConstraint = new Constraint(colliderA, colliderB, ConstraintType::SLIDER);
			App->scene_intro->constraints.push_back(newConstraint);
			newConstraint->distance.Set(dist.getX(), dist.getY(), dist.getZ());

			btVector3 pivotinA, pivotinB;
			SetPivots(dist, pivotinA, pivotinB, colliderA, colliderB);
			btTransform localA;
			btTransform localB;
			localA.setIdentity();
			localB.setIdentity();
			localA.setOrigin(pivotinA);
			localB.setOrigin(pivotinB);
			btSliderConstraint* constraint = new btSliderConstraint(*colliderA->body.GetBody(), *colliderB->body.GetBody(), localA, localB, linearreference);
			
			newConstraint->LowerLinLimit = 0.f;
			newConstraint->UpperLinLimit = 1.f;
			newConstraint->LowerAngLimit = 0.f;
			newConstraint->UpperAngLimit = 1.f;

			constraint->setLowerLinLimit(0.f);
			constraint->setUpperLinLimit(1.f);
			constraint->setLowerAngLimit(0.f);
			constraint->setUpperAngLimit(1.f);

			world->addConstraint(constraint);
			SliderConstraints.push_back(constraint);
			newConstraint->ConstraintPointer = constraint;

			return constraint;
		}
	}
	return nullptr;
}

btConeTwistConstraint* ModulePhysics3D::AddConstraintCone(GameObject* bodyA, GameObject* bodyB, btVector3& distance)
{
	if (bodyA && bodyB)
	{
		Collider* colliderA = (Collider*)bodyA->GetComponent(ComponentType::COLLIDER);
		Collider* colliderB = (Collider*)bodyB->GetComponent(ComponentType::COLLIDER);
		if (colliderA && colliderB && colliderA != colliderB)
		{
			Constraint* newConstraint = new Constraint(colliderA, colliderB, ConstraintType::CONE);
			App->scene_intro->constraints.push_back(newConstraint);
			newConstraint->distance.Set(distance.getX(), distance.getY(), distance.getZ());

			btVector3 pivotinA, pivotinB;
			SetPivots(distance, pivotinA, pivotinB, colliderA, colliderB);
			btTransform localA;
			btTransform localB;
			localA.setIdentity();
			localB.setIdentity();
			localA.setOrigin(pivotinA);
			localB.setOrigin(pivotinB);

			btConeTwistConstraint* constraint = new btConeTwistConstraint(*colliderA->body.GetBody(), *colliderB->body.GetBody(), localA, localB);
			

			world->addConstraint(constraint);
			ConeConstraints.push_back(constraint);
			newConstraint->ConstraintPointer = constraint;

			return constraint;
		}
	}
	return nullptr;
}

void ModulePhysics3D::SetPivots(btVector3& dist, btVector3& pivotinA, btVector3& pivotinB, Collider* colliderA, Collider* colliderB)
{
	
	pivotinA.setValue(0, 0, 0);
	pivotinB.setValue(0, 0, 0);
	vec3 pos1 = colliderA->body.GetPos();
	vec3 pos2 = colliderB->body.GetPos();
	float3 distance;
	distance.Set(dist.getX(), dist.getY(), dist.getZ());
	if (pos1.x > pos2.x && distance.x > 0)
		pivotinA.setX(distance.x);
	else if (pos1.x < pos2.x && distance.x < 0)
		pivotinA.setX(distance.x);
	else
		pivotinB.setX(distance.x);

	if (pos1.y > pos2.y && distance.y > 0)
		pivotinA.setY(distance.y);
	else if (pos1.y < pos2.y && distance.y < 0)
		pivotinA.setY(distance.y);
	else
		pivotinB.setY(distance.y);

	if (pos1.z > pos2.z && distance.z > 0)
		pivotinA.setZ(distance.z);
	else if (pos1.z < pos2.z && distance.z < 0)
		pivotinA.setZ(distance.z);
	else
		pivotinB.setZ(distance.z);

}
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