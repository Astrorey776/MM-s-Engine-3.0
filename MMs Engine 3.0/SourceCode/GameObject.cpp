#include "GameObject.h"
#include "Primitive.h"
#include "Application.h"

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
	to_delete = false;
	active = true;
	Name = "NewGameObject";
	parent = nullptr;
	isSelected = false;
	LCG();
	LCG rand;
	UID = rand.Int();
	parentUID = 0;
	displayAABB = true;
	currentAABB = nullptr;
	culled = false;

}

GameObject::GameObject(const char* name, GameObject* node)
{
	to_delete = false;
	active = true;
	Name = name;
	parent = node;
	isSelected = false;
	LCG();
	LCG rand;
	UID = rand.Int();
	displayAABB = false;
	if (parent != nullptr)
	{
		parent->childs.push_back(this);
		parentUID = parent->UID;
	}
	currentAABB = nullptr;
	culled = false;

}

GameObject::~GameObject()
{

	HideAABB();

	for (int i = 0; i < Components.size(); i++)
	{
		if (Components[i] != nullptr)
		{
			//Components[i]->type;
			delete Components[i];

		}
	}
	Components.clear();
}

bool GameObject::Update(float dt)
{
	bool ret = true;
	this;
	if (!to_delete && active)
	{
		ComponentMesh* myMesh = (ComponentMesh*)GetComponent(ComponentType::MESH);
		ComponentTransform* myTrans = (ComponentTransform*)GetComponent(ComponentType::TRANSFORM);
		
		if (myMesh != nullptr)
		{
			obb = myMesh->GetAABB();
			if (myTrans != nullptr)
			{
				
				obb.Transform(myTrans->global_transform);
			
				aabb.SetNegativeInfinity();
				aabb.Enclose(obb);
				
				HideAABB();
				if (displayAABB)
				{
					DisplayAABB();
				}
			
				
				if (App->scene_intro->culling != nullptr)
				{
					if (!App->scene_intro->culling->ContainsAABB(aabb))
						culled = true;
				}
			}

		}
	
		

		for (int i = 0; i < Components.size(); i++)
		{

			if(Components[i]->active)Components[i]->Update(dt);
		}
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
	case ComponentType::CAMERA: {ComponentCamera* camera = new ComponentCamera(this); newComponent = camera; break; }
	case ComponentType::COLLIDER: {Collider* collider = new Collider(this); newComponent = collider; break; }
	}
	Components.push_back(newComponent);

	return newComponent;
}

Component* GameObject::GetComponent(ComponentType type)
{
	Component* newComponent = nullptr;
	int counter = 0;
	for (int i = 0; i < Components.size(); i++)
	{
		if (type == Components[i]->type)
		{
			newComponent = Components[i];
		}
	}

	return newComponent;

}

void GameObject::HideAABB()
{
	if (currentAABB != nullptr)
	{
		currentAABB->to_delete = true;
		currentAABB = nullptr;
	}
}

void GameObject::DisplayAABB()
{
	currentAABB = App->PrimManager->CreateAABB(&aabb);
}

//*************************		Component
Component::Component()
{

	owner = nullptr;
	active = false;
	reference = nullptr;
	type = ComponentType::NONE;
}

Component::~Component()
{
	LOG("deleting component");
	if(reference != nullptr)
	reference->references--;
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
//*************************		Collider
Collider::Collider(GameObject* ObjectOwner) : Component()
{
	type = ComponentType::COLLIDER;
	collidertype = ColliderType::NONE;
	active = true;
	owner = ObjectOwner;
	body.SetBody(ObjectOwner, 1, collidertype);
}

Collider::~Collider()
{
	for (int i = 0; i < constraints.size(); i++)
	{
		if (constraints[i] != nullptr)
		{
			delete constraints[i];
			i--;
		}
	
	}
	constraints.clear();
}

bool Collider::Update(float dt)
{
	//float* matrix;
	//body.GetTransform(matrix);
	//btVector3 min, max;
	//btTransform t;
	//t.setIdentity();
	//body.GetBody()->getAabb(min, max);
	//body.GetBody()->getCollisionShape()->getAabb(t, min, max);
	//
	return true;
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

	triggerNormals = false;
	GraphicNormals = nullptr;
}

ComponentMesh::~ComponentMesh()
{
	LOG("deleting component mesh");
	if (GraphicNormals != nullptr)
	{
		GraphicNormals->to_delete = true;
	}

	/*glDeleteBuffers(1, &id_index);
	glDeleteBuffers(1, &id_normals);
	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_tex);

	if (vertex != nullptr)
	{
		delete[] vertex;
		vertex = nullptr;
	}
	if (index != nullptr)
	{
		delete[] index;
		index = nullptr;
	}
	if (normals != nullptr)
	{
		delete[] normals;
		normals = nullptr;
	}
	
	if (texCoords != nullptr)
	{
		delete[] texCoords;
		texCoords = nullptr;
	}*/

}

bool ComponentMesh::Update(float dt)
{
	bool ret = true;
	if (!owner->culled)
	{
		ComponentMaterial* material = nullptr;
		ComponentTransform* transform = nullptr;
		material = (ComponentMaterial*)owner->GetComponent(ComponentType::MATERIAL);
		transform = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);

		/*for (int i = 0; i < owner->Components.size(); i++)
		{
			if (owner->Components[i]->type == ComponentType::MATERIAL)
			{
				material = (ComponentMaterial*)owner->Components[i];
			}
			if (owner->Components[i]->type == ComponentType::TRANSFORM)
			{
				transform = (ComponentTransform*)owner->Components[i];
			}
		}*/


		glPushMatrix();
		if (transform != nullptr)
		{
			glMultMatrixf(transform->global_transform.Transposed().ptr());
		}
		else
		{
			glLoadIdentity();
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if (id_vertex != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
			glVertexPointer(3, GL_FLOAT, 0, NULL);
		}

		if (id_normals != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, id_normals);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}
		if (id_tex != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, id_tex);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}



		if (material != nullptr && material->hastexture)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_CULL_FACE);

			glActiveTexture(GL_TEXTURE0);
			if (material->checkers || material->texbuffer == 0 || !material->active)
				glBindTexture(GL_TEXTURE_2D, App->scene_intro->texName);
			else
				glBindTexture(GL_TEXTURE_2D, material->texbuffer);
		}
		else
		{
			if (material != nullptr && material->checkers)

				glEnable(GL_TEXTURE_2D);
			glEnable(GL_CULL_FACE);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, App->scene_intro->texName);

		}


		if (id_index != 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
			glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);
		}


		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();

	}
	owner->culled = false;

	return ret;
}

void ComponentMesh::DisplayNormals()
{
	LOG("Dispaying normals");
	float* normal_array;
	normal_array = new float[num_vertex * 6];

	//if (num_vertex < 5000)
		for (int i = 0; i < num_vertex; i++)
		{
			vec3 origin(vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
			vec3 destination(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
			destination += origin;

			float distance_to_adjust = 0.5;
			vec3 distance;
			distance.x = (origin.x + (destination.x - origin.x) * distance_to_adjust);
			distance.y = (origin.y + (destination.y - origin.y) * distance_to_adjust);
			distance.z = (origin.z + (destination.z - origin.z) * distance_to_adjust);


			normal_array[i * 6] = origin.x;
			normal_array[i * 6 + 1] = origin.y;
			normal_array[i * 6 + 2] = origin.z;
			normal_array[i * 6 + 3] = distance.x;
			normal_array[i * 6 + 4] = distance.y;
			normal_array[i * 6 + 5] = distance.z;

		}

		GraphicNormals = App->PrimManager->CreateNormalVects(normal_array, num_vertex*2);
		ComponentTransform* transform = nullptr;

		for (int i = 0; i < owner->Components.size(); i++)
		{
			
			if (owner->Components[i]->type == ComponentType::TRANSFORM)
			{
				transform = (ComponentTransform*)owner->Components[i];
				GraphicNormals->SetPos(transform->pos.x, transform->pos.y, transform->pos.z);
				GraphicNormals->Scale(transform->scale.x, transform->scale.y, transform->scale.z);
			}
		}

}

void ComponentMesh::SetAABB()
{
	bbox.SetNegativeInfinity();
	float3* vertexvert = (float3*)vertex;
	bbox.Enclose(vertexvert, num_vertex);

}
AABB ComponentMesh::GetAABB()
{
	return bbox;
}
void ComponentMesh::HideNormals()
{
	if (GraphicNormals != nullptr)
	{
		GraphicNormals->to_delete = true;
		GraphicNormals = nullptr;
	}
	
	
	LOG("HideNormals");
}
//*************************		ComponentMaterial

ComponentMaterial::ComponentMaterial(GameObject* ObjectOwner) : Component()
{
	type = ComponentType::MATERIAL;
	active = true;
	owner = ObjectOwner;
	texbuffer = 0;
	hastexture = false;
	checkers = false;
	texture_w = 0;
	texture_h = 0;
}

ComponentMaterial::~ComponentMaterial()
{
	LOG("Deleting Component Material");

	//glDeleteTextures(1, &(GLuint)texbuffer);
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

	pos.Set(0, 1, 0);
	scale.Set(1, 1, 1);
	rot.Set(0, 0, 0, 1);
	local_transform = local_transform.identity;
	global_transform = AcumulateparentTransform();
	should_update = false;
	using_guizmo = false;
}

ComponentTransform::~ComponentTransform()
{
	LOG("Deleting Component Transform");
}

bool ComponentTransform::Update(float dt)
{
	bool ret = true;
	//UpdatePos(pos.x, pos.y, pos.z);
	//vec3 axis(1, 0, 0);
	Collider* collider = nullptr;

	
	
	if (should_update)
	{
		local_transform = float4x4::FromTRS(pos, rot, scale);
	}

	global_transform = AcumulateparentTransform();
	float4x4 worldtolocal = local_transform * global_transform.Inverted();
	if (!should_update && !using_guizmo)
	{
		if (owner != nullptr)
		{
			collider = (Collider*)owner->GetComponent(ComponentType::COLLIDER);
			if (collider)
			{
				float4x4 collider_trans;
				collider->body.GetTransform(collider_trans.ptr());
				collider_trans.Transpose();
				collider_trans = collider->body.GetTransform(collider_trans);

				collider_trans = collider_trans * collider->body.TransformMatrix.Inverted();
				float3 globscale, globscale2, globcolpos;
				Quat globcolrot;
				global_transform.Decompose(globcolpos, globcolrot, globscale);
				global_transform = collider_trans * collider->body.localTransform;
				global_transform.Decompose(globcolpos, globcolrot, globscale2);

				//global_transform = float4x4::FromTRS(globcolpos, globcolrot, globscale2);

				local_transform = worldtolocal * global_transform;
				local_transform.Decompose(pos, rot, scale);
				//btQuaternion quat(rot.x, rot.y, rot.z, rot.w);
			}
		}

	}
	else
	{
		collider = (Collider*)owner->GetComponent(ComponentType::COLLIDER);
		if (collider)
		{

			float4x4 colltrans = global_transform * collider->body.localTransform.Inverted();
			/*float3 colscale, colpos;
			Quat colrot;
			global_transform.Decompose(colpos, colrot, colscale);
			btVector3 size;
			size.setValue(colscale.x, colscale.y, colscale.z);

			collider->body.GetBody()->getCollisionShape()->setLocalScaling(size);*/
			collider->body.SetTransform(colltrans);

		}
	}
	using_guizmo = false;
	should_update = false;

	//UpdateScale(scale.x, scale.y, scale.z);
	return ret;
}

float4x4 ComponentTransform::AcumulateparentTransform()
{
	float4x4 parentmat;
	parentmat = parentmat.identity;
	ComponentTransform* parentransform = nullptr;
	if (owner->parent != nullptr)
	{
	//	LOG("%s, %s", owner->Name.c_str(), owner->parent->Name.c_str());
			for (int i = 0; i < owner->parent->Components.size(); i++)
			{
				
				if (owner->parent->Components[i]->type == ComponentType::TRANSFORM)
				{
					parentransform = (ComponentTransform*)owner->parent->Components[i];
				}
			}
			if (owner == owner->parent)
				LOG("Stack Overflow")

			if(parentransform != nullptr)
		parentmat = parentransform->AcumulateparentTransform();
	}

	return parentmat * local_transform;
}

void ComponentTransform::UpdateRotation(Quat quat)
{
	local_transform = local_transform * quat;
}



void ComponentTransform::SetPos(float x, float y, float z)
{
	pos.Set(pos.x + x, pos.y + y, pos.z + z);
	should_update = true;

	
}

void ComponentTransform::SetRotation(Quat quat)
{
	rot = rot * quat;
	should_update = true;


}

void ComponentTransform::Scale(float x, float y, float z)
{
	scale.Set(scale.x + x, scale.y + y, scale.z + z);
	should_update = true;


}
void ComponentTransform::UpdateFromGuizmo(float4x4 newAll) 
{
	float4x4 initmat = global_transform.Transposed();

	float3 trans, init_trans, init_scale, scale;
	Quat rot, init_rot;
	newAll.Decompose(trans, rot, scale);
	initmat.Decompose(init_trans, init_rot, init_scale);

	//pos.Set(trans.x - init_trans.x, trans.y - init_trans.y, trans.z - init_trans.z);

	pos.Set(trans.x, trans.y, trans.z);
	//rot.Set(rotation.x, rotation.y, rotation.z, rotation.w);
	//scale.Set(scaling.x, scaling.y, scaling.y);
}

//*************************		ComponentCamera

ComponentCamera::ComponentCamera(GameObject* ObjectOwner) :Component() {
	type = ComponentType::CAMERA;
	active = true;
	owner = ObjectOwner;
	cullingActive = false;
	
	aspectRatio = 1.73; 
	frustrum.front = float3(0.0f, 0.0f, 1.0f);
	frustrum.up = float3(0.0f, 1.f, 0.0f);

	frustrum.nearPlaneDistance = 4;
	frustrum.farPlaneDistance = 300;
	frustrum.pos = float3(0.0f,0.0f,0.0f);
	frustrum.type = FrustumType::PerspectiveFrustum;

	frustrum.horizontalFov = (65* DEGTORAD);//This will stay as it is
	frustrum.verticalFov = (65 * DEGTORAD) / aspectRatio; //This will be adaptable
	planes = new math::Plane[6];
	frustrum.GetPlanes(planes);
	print = true;
	SetFOV(60);
}

ComponentCamera::~ComponentCamera()
{
	if (this == App->scene_intro->culling)
	{
		App->scene_intro->culling = nullptr;
	}
	if (planes != nullptr)
	{
		delete[] planes;
		planes = nullptr;
	}

}

bool ComponentCamera::Update(float dt)
{
	bool ret = true;
	
	PrintFrustrum();
	return ret;
}

void ComponentCamera::PrintFrustrum() 
{
	//updateFrustrum();
	UpdateOrientation();
	UpdatePos();
	
	if (print)
	{
		//frustrum.pos = float3(0.0f, 1.0f, 0.0f);
		float3 corners[8];
		frustrum.GetCornerPoints(corners);
		CreateFrustrum(corners);
	}

}

void ComponentCamera::CreateFrustrum(float3* corners) 
{
	glBegin(GL_LINES);
	glVertex3fv(corners[0].ptr()); glVertex3fv(corners[1].ptr());
	glVertex3fv(corners[0].ptr()); glVertex3fv(corners[2].ptr());
	glVertex3fv(corners[0].ptr()); glVertex3fv(corners[4].ptr());
	glVertex3fv(corners[3].ptr()); glVertex3fv(corners[1].ptr());
	glVertex3fv(corners[3].ptr()); glVertex3fv(corners[2].ptr());
	glVertex3fv(corners[3].ptr()); glVertex3fv(corners[7].ptr());
	glVertex3fv(corners[5].ptr()); glVertex3fv(corners[1].ptr());
	glVertex3fv(corners[5].ptr()); glVertex3fv(corners[4].ptr());
	glVertex3fv(corners[5].ptr()); glVertex3fv(corners[7].ptr());
	glVertex3fv(corners[6].ptr()); glVertex3fv(corners[2].ptr());
	glVertex3fv(corners[6].ptr()); glVertex3fv(corners[4].ptr());
	glVertex3fv(corners[6].ptr()); glVertex3fv(corners[7].ptr());
	glEnd();

}

void ComponentCamera::updateFrustrum() 
{
	float3 rotation = float3(0.0f, 0.0f, 0.0f);
	rotation *= DEGTORAD;
	float4x4 toSend = float4x4::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	frustrum.SetWorldMatrix(toSend.Float3x4Part());
	//frustrum.pos = float3(0.0f, 0.0f, 0.0f);
}

float ComponentCamera::GetFOV() 
{
	//The fov is in radians so easier to understand for the user:
	return frustrum.verticalFov * RADTODEG;
}
float ComponentCamera::GetHorizontalFov()
{
	//The fov is in radians so easier to understand for the user:
	return frustrum.horizontalFov * RADTODEG;
}

void ComponentCamera::SetFOV(float FOV)
{
	//float aspecratio;
	FOV = FOV * DEGTORAD; //now we pas to rad again
	//aspecratio = frustrum.AspectRatio();
	frustrum.verticalFov = FOV;
	frustrum.horizontalFov = 2.f * Atan(Tan(FOV * 0.5f) * aspectRatio/*aspecratio*/);
	//frustrum.GetPlanes
}

void ComponentCamera::UpdatePos()
{
	ComponentTransform* CameraTransform = nullptr;
	CameraTransform = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);
	if(CameraTransform!= nullptr)
	{
		float4x4 abc = CameraTransform->AcumulateparentTransform();
		//frustrum.Transform(abc);
		frustrum.SetWorldMatrix(abc.Float3x4Part());
		
	}
	

}

void ComponentCamera::UpdateOrientation() 
{
	/*float3 rotation = {0,0,0};
	ComponentTransform* CameraTransform = nullptr;
	CameraTransform = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);
	
	if (CameraTransform != nullptr)
	{
		frustrum.Tra
	
		//frustrum.front = CameraTransform->transform.Transposed().WorldZ().Normalized();

		//frustrum.Transform(CameraTransform->rot);
	}*/
	

}

bool ComponentCamera::ContainsAABB(const AABB refBox) const
{
	bool ret = true;
	float3 vCorner[8];
	
	refBox.GetCornerPoints(vCorner);

	frustrum.GetPlanes(planes);
	int PlanesCount = 0;
	int insideCount = 0;
	for (int p = 0; p < 6; ++p) {
		PlanesCount = 0;
		for (int i = 0; i < 8; ++i) {
		//	 test this point against the planes
			//if (planes[p].IsOnPositiveSide(vCorner[i]))  //<-- “IsOnPositiveSide” from MathGeoLib
	
			if (planes[p].normal.Dot(vCorner[i]) - planes[p].d >= 0.f)
				PlanesCount++;
	
		}
		if (PlanesCount < 8)
			insideCount++;
	}
	//if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	//LOG("%d", iInCount);
		if (insideCount < 6)
			ret = false;
	return ret;
}

Constraint::Constraint(Collider* A, Collider* B, ConstraintType T)
{
	colliderA = A;
	colliderB = B;
	type = T;
	colliderA->constraints.push_back(this);
	colliderB->constraints.push_back(this);
	ConstraintPointer = nullptr;
}

Constraint::~Constraint()
{
	for (int i = 0; i < colliderA->constraints.size(); i++)
	{
		if (colliderA->constraints[i] = this)
		{
			colliderA->constraints.erase(colliderA->constraints.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < colliderB->constraints.size(); i++)
	{
		if (colliderB->constraints[i] = this)
		{
			colliderB->constraints.erase(colliderB->constraints.begin() + i);
			i--;
		}
	}

	if (ConstraintPointer != nullptr)
		App->Physics->world->removeConstraint(ConstraintPointer);
	ConstraintPointer = nullptr;
}