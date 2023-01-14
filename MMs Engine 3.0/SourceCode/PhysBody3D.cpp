#include "PhysBody3D.h"
#include "Globals.h"
#include "Application.h"
#include "Primitive.h"
#include "GameObject.h"
#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "MathGeoLib/include/MathGeoLib.h"

// ---------------------------------------------------------
PhysBody3D::PhysBody3D()
	: body(nullptr)
	, colShape(nullptr)
	, motionState(nullptr)
	, parentPrimitive(nullptr)
{
	TransformMatrix = float4x4::identity;
	BodyMass = 0;
	is_sensor = false;
}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
	if (HasBody() == true)
	{
		App->Physics->RemoveBodyFromWorld(body);
		delete body;
		delete colShape;
		delete motionState;
	}
}

void PhysBody3D::SetBody(PrimSphere* primitive, float mass)
{
	SetBody(new btSphereShape(/*primitive->GetRadius()*/1),primitive, mass);
}

void PhysBody3D::SetBody(Cube* primitive, float mass)
{
	const btVector3 vec(primitive->size.x / 2, primitive->size.y / 2, primitive->size.z / 2);
	SetBody(new btBoxShape(btVector3(vec)), primitive, mass);
}

//void PhysBody3D::SetBody(GameObject* obj, float mass)
//{
//	
//	ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(ComponentType::MESH);
//	if (mesh != nullptr)
//	{
//		AABB bbox = mesh->GetAABB();
//		float3 corners[8];
//		bbox.GetCornerPoints(corners);
//		float width = corners[0].Distance(corners[4]);
//		float heigth = corners[1].Distance(corners[3]);
//		float depth = corners[3].Distance(corners[2]);
//		const btVector3 vec(width / 2, heigth / 2, depth / 2);
//		SetBody(new btBoxShape(btVector3(vec)), obj, mass);
//	}
//	else
//	{
//		const btVector3 vec(1 / 2, 1 / 2, 1 / 2);
//		SetBody(new btBoxShape(btVector3(vec)), obj, mass);
//	}
//	
//}

void PhysBody3D::SetBody(GameObject* obj, float mass, ColliderType type12)
{
	if (type12 == ColliderType::NONE)
	{
		type = ColliderType::NONE;
	}
	if (type12 == ColliderType::BOX)
	{
		type = ColliderType::BOX;

		ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(ComponentType::MESH);
		if (mesh != nullptr)
		{
			AABB bbox = mesh->GetAABB();
			float3 corners[8];
			bbox.GetCornerPoints(corners);
			float width = corners[0].Distance(corners[4]);
			float heigth = corners[1].Distance(corners[3]);
			float depth = corners[3].Distance(corners[2]);
			const btVector3 vec(width / 2, heigth / 2, depth / 2);
			SetBody(new btBoxShape(btVector3(vec)), obj, mass);
		}
		else
		{
			const btVector3 vec(1 / 2, 1 / 2, 1 / 2);
			SetBody(new btBoxShape(btVector3(vec)), obj, mass);
		}
	}

	if (type12 == ColliderType::SPHERE) 
	{
		type = ColliderType::SPHERE;

		ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(ComponentType::MESH);
		if (mesh != nullptr)
		{

			AABB bbox = mesh->GetAABB();
			float3 corners[8];
			bbox.GetCornerPoints(corners);
			float width = corners[0].Distance(corners[4]);
			SetBody(new btSphereShape(width/2), obj, mass);
		}
		else
		{
			SetBody(new btSphereShape(1 / 2), obj, mass);
		}
	}

	if (type12 == ColliderType::CAPSULE)
	{
		type = ColliderType::CAPSULE;

		ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(ComponentType::MESH);
		if (mesh != nullptr)
		{
			AABB bbox = mesh->GetAABB();
			float3 corners[8];
			bbox.GetCornerPoints(corners);
			float width = corners[0].Distance(corners[4]);
			float heigth = corners[1].Distance(corners[3]);
			float depth = corners[3].Distance(corners[2]);
			SetBody(new btCapsuleShape(width /2, heigth / 2), obj, mass);
		}
		else
		{
			SetBody(new btCapsuleShape(1 / 2, 1/2), obj, mass);
		}
	}
}

void PhysBody3D::SetBody(PrimCylinder* primitive, float mass)
{
	/*const btVector3 vec(primitive->GetRadius(), primitive->GetHeight()/2, mass);
	SetBody(new btCylinderShape(vec),primitive, mass);*/
}


bool PhysBody3D::HasBody() const
{
	return body != nullptr;
}

btRigidBody * PhysBody3D::GetBody() const
{
	return body;
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if (HasBody() == false)
		return;

	body->getWorldTransform().getOpenGLMatrix(matrix);
}

float4x4 PhysBody3D::GetTransform(float4x4 mat)
{
	

	if (HasBody() == false)
		return mat;

	float3 pos, scale;
	Quat rot;
	mat.Decompose(pos, rot, scale);

	float4x4 newtrans = globalTransform * TransformMatrix;
	float3 pos2, scale2;
	Quat rot2;
	newtrans.Decompose(pos2, rot2, scale2);
	mat = float4x4::FromTRS(pos, rot, scale2);
	return mat;
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if (HasBody() == false)
		return;

	btTransform trans;
	trans.setFromOpenGLMatrix(matrix);
	body->setWorldTransform(trans);
	body->activate();
}

void PhysBody3D::SetTransform(float4x4 matrix)
{
	if (HasBody() == false)
		return;
	globalTransform = matrix;
	float4x4 newtrans = globalTransform * TransformMatrix;
	float3 pos, scale;
	Quat rot;
	newtrans.Decompose(pos, rot, scale);
	scale.Set(1, 1, 1);
	newtrans = float4x4::FromTRS(pos, rot, scale);
	btTransform trans;
	trans.setFromOpenGLMatrix(newtrans.Transposed().ptr());
	body->setWorldTransform(trans);
	body->activate();
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	if (HasBody() == false)
		return;

	btTransform trans = body->getWorldTransform();
	trans.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(trans);
	body->activate();
}

void PhysBody3D::SetRotation(btQuaternion quat)
{
	if (HasBody() == false)
		return;

	btTransform trans = body->getWorldTransform();
	trans.setRotation(quat);
	body->setWorldTransform(trans);
	body->activate();
}

void PhysBody3D::SetSpeed(vec3 speed)
{
	Stop();
	Push(speed);
}

void PhysBody3D::Push(vec3 force)
{
	if (HasBody())
	{
		body->activate();
		body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
}

void PhysBody3D::Stop()
{
	if (HasBody())
		body->clearForces();
}

void PhysBody3D::SetBody(btCollisionShape * shape, Primitive* parent, float mass)
{
	assert(HasBody() == false);

	parentPrimitive = parent;

	colShape = shape;

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&parent->transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

	body = new btRigidBody(rbInfo);

	body->setUserPointer(this);

	App->Physics->AddBodyToWorld(body);
}

void PhysBody3D::SetBody(btCollisionShape* shape, GameObject* parent, float mass)
{
	assert(HasBody() == false);

	parentGameObject = parent;

	
	colShape = shape;

	btTransform startTransform;
	ComponentTransform* comp_transform = nullptr;
	comp_transform = (ComponentTransform*)parent->GetComponent(ComponentType::TRANSFORM);

	ComponentMesh* mesh = (ComponentMesh*)parent->GetComponent(ComponentType::MESH);
	
	OBB obb = mesh->GetAABB();
	obb.Transform(comp_transform->global_transform);
	float3 pos;
	float3 scale;
	Quat rot;
	comp_transform->global_transform.Decompose(pos, rot, scale);
	pos = { obb.pos.x, obb.pos.y, obb.pos.z };
	
	//App->PrimManager->CreateSphere(1, 20, 20, pos);
	

	btVector3 size;
	size.setValue(scale.x, scale.y, scale.z);

	colShape->setLocalScaling(size);
	scale.Set(1, 1, 1);

	float4x4 transform = float4x4::FromTRS(pos, rot, scale);
	float4x4 inversedtransform = comp_transform->global_transform;
	localTransform = transform.Inverted() * inversedtransform;	
	/*
	transform->SetPos(bbox.CenterPoint().x, bbox.CenterPoint().y, bbox.CenterPoint().z);
	transform->Update(0);*/
	globalTransform = transform;
	startTransform.setFromOpenGLMatrix(transform.Transposed().ptr());

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);
	BodyMass = mass;
	motionState = new btDefaultMotionState(startTransform);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

	body = new btRigidBody(rbInfo);

	body->setUserPointer(this);

	App->Physics->AddBodyToWorld(body);
	
}
const vec3 PhysBody3D::GetPos() const
{
	float matrix[16];
	GetTransform(matrix);
	vec3 pos(matrix[12], matrix[13], matrix[14]);
	return pos;
}

vec3 PhysBody3D::GetForwardVector() const
{
	mat4x4 transform;
	body->getWorldTransform().getOpenGLMatrix(&transform); //Here we pick the rotation and the translation 
	mat3x3 rotation(transform);       //And here we save the translation
	
	vec3 forward(0.f, 0.f, 1.f);
	forward = rotation * forward;

	return forward;
}

vec3 PhysBody3D::GetaAboveVector() const
{
	mat4x4 transform;
	body->getWorldTransform().getOpenGLMatrix(&transform); //Here we pick the rotation and the translation 
	mat3x3 rotation(transform);       //And here we save the translation

	vec3 forward(0.f, 1.f, 0.f);
	forward = rotation * forward;

	return forward;
}

void PhysBody3D::SetAsSensor(bool is_sensor)
{
	if (this->is_sensor != is_sensor)
	{
		this->is_sensor = is_sensor;
		if (is_sensor == true)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

bool PhysBody3D::IsSensor() const
{
	return is_sensor;
}