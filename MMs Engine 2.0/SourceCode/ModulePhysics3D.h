#ifndef __MODULEPHYSICS__
#define __MODULEPHYSICS__

#include "Module.h"
#include "Globals.h"

#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"

#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class GameObject;
class Collider;
class DebugDrawer;
struct VehicleInfo;
//class  PhysBody3D;

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(Application* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void AddBodyToWorld(btRigidBody* body);
	void RemoveBodyFromWorld(btRigidBody* body);

	//PhysBody3D* RayCast(const vec3& Origin, const vec3& Direction, vec3& HitPoint = vec3());

	//TODO 1: Implement the code to add a Point to Point constraint ( btPoint2PointConstraint )
	//void AddConstraintP2P(const Primitive& bodyA, const Primitive& bodyB, ...);
	
	btPoint2PointConstraint* AddConstraintP2P(const Primitive& bodyA, const Primitive& bodyB, btVector3& pivotInA, btVector3& pivotInB);
	btPoint2PointConstraint* AddConstraintP2P(GameObject* bodyA, GameObject* bodyB, btVector3& distance);

	//TODO 3: Implement the code to add a Hinge constraint ( btHingeConstraint )

	btHingeConstraint* AddConstraintHinge(const Primitive & bodyA, const Primitive & bodyB, btVector3& pivotInA, btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB);
	btHingeConstraint* AddConstraintHinge(GameObject* bodyA, GameObject* bodyB, btVector3& distance, btVector3& axisInA, btVector3& axisInB);

	btSliderConstraint* AddConstraintSlider(const Primitive& bodyA, const Primitive& bodyB, btTransform& frameinA, btTransform& frameinB, bool linearreference = true);
	btSliderConstraint* AddConstraintSlider(GameObject* bodyA, GameObject* bodyB, btVector3& distance, bool linearreference = true);

	btConeTwistConstraint* AddConstraintCone(GameObject* bodyA, GameObject* bodyB, btVector3& distance);

	PhysVehicle3D* AddVehicle(const VehicleInfo& info);
	// Create map walls

	void SetPivots(btVector3& dist, btVector3& pivotinA, btVector3& pivotinB, Collider* colliderA, Collider* colliderB);
	
	btDiscreteDynamicsWorld*			world;
	float gravity;

private:
	btDefaultCollisionConfiguration*	collision_conf;
	btCollisionDispatcher*				dispatcher;
	btBroadphaseInterface*				broad_phase;
	btSequentialImpulseConstraintSolver* solver;

	DebugDrawer*						debug_draw;
	std::vector<btHingeConstraint*>			HingeConstraints;
	std::vector<btSliderConstraint*>			SliderConstraints;
	std::vector<btConeTwistConstraint*>			ConeConstraints;
	std::vector<btCollisionShape*>		shapes;
	std::vector<PhysVehicle3D*>		vehicles;
	btDefaultVehicleRaycaster*			vehicle_raycaster;
	// Change color of map walls
	int color = 0;
public:
	std::vector<btPoint2PointConstraint*>	P2PConstraints;

};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line()
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	PrimLine line;
	Primitive point;
	
};
#endif // __MODULEPHYSICS__
