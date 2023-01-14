#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GameObject.h"
#include "MousePicking.h"

#include "MathGeoLib/include/MathGeoLib.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	cameraOBJ = nullptr;
	cameraTrans = nullptr;
	cameraComp = nullptr;
	frustrumFollow = true;

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(25.0f, 5.0f, 25.0f);
	Reference = vec3(0.0f, 10.0f, 0.0f);

	CalculateViewMatrix();
	
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	cameraOBJ = new GameObject("Camera", nullptr);
	cameraTrans = (ComponentTransform*)cameraOBJ->CreateComponent(ComponentType::TRANSFORM);
	cameraComp = (ComponentCamera*)cameraOBJ->CreateComponent(ComponentType::CAMERA);
	cameraComp->frustrum.nearPlaneDistance = 0.1;

	cameraComp->frustrum.nearPlaneDistance = 1;
	cameraComp->print = false;
	background.Set(0.f, 0, 0.f, 1.f);
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (App->scene_intro->spherecamera != nullptr)
	{
		App->scene_intro->spherecamera->body.SetPos(App->camera->Position.x+(App->camera->Z.x*1.1), App->camera->Position.y +(App->camera->Z.y*1.1), App->camera->Position.z +(App->camera->Z.z*1.1));
	}
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!
	cameraOBJ->Update(dt);
	vec3 newPos(0,0,0);
	float speed = 4.0f * dt;
	
	
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	//if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	//if(App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	if(App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		App->UI->guizmo_type = ImGuizmo::OPERATION::TRANSLATE;
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
	{
		App->UI->guizmo_type = ImGuizmo::OPERATION::ROTATE;
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT)
	{
		App->UI->guizmo_type = ImGuizmo::OPERATION::SCALE;
	}

	if (App->input->mouse_z > 0) 
	{
		if(App->input->mouse_x > App->UI->img_corner.x && App->input->mouse_x < (App->UI->img_corner.x + App->UI->image_size.x) && 
		   App->input->mouse_y > App->UI->img_corner.y && App->input->mouse_y < (App->UI->img_corner.y + App->UI->image_size.y))
		{
			newPos -= Z * 20.0f * dt;
		}
		
	}
	if (App->input->mouse_z < 0)
	{
		if (App->input->mouse_x > App->UI->img_corner.x && App->input->mouse_x < (App->UI->img_corner.x + App->UI->image_size.x) &&
			App->input->mouse_y > App->UI->img_corner.y && App->input->mouse_y < (App->UI->img_corner.y + App->UI->image_size.y))
		{
			newPos += Z * 20.0f * dt;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) 
	{
		LookAt(App->UI->direction_camera); //when no fbx this will look at 0,0,0
	}
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		Reference = App->UI->direction_camera;

		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}

	Position += newPos;
	Reference += newPos;


	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		// TODO (Homework): Rotate the camera with the mouse
		vec3 Forward = -Z;

		Forward = rotate(Forward, dx, Y);
		Forward = rotate(Forward, dy, X);

		LookAt(Forward + Position);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{

	/*	float mouseNormX = App->input->GetMouseX();
		mouseNormX /= App->window->windowSize.x;

		float mouseNormY = App->window->windowSize.y - App->input->GetMouseY();
		mouseNormY /= App->window->windowSize.y;

		mouseNormX = (mouseNormX - 0.5) / 0.5;
		mouseNormY = (mouseNormY - 0.5) / 0.5;*/
		float mouseNormX = (App->input->GetMouseX() - App->UI->img_corner.x )/ App->UI->image_size.x;
		float mouseNormY = (App->input->GetMouseY() - App->UI->img_corner.y) / App->UI->image_size.y;

		mouseNormX = (mouseNormX - 0.5) / 0.5;
		mouseNormY = -(mouseNormY - 0.5) / 0.5;

		//LOG("x = %f, y = %f", App->UI->img_corner.x, App->UI->img_corner.y);
		if (mouseNormX >= -1 && mouseNormX <= 1 && mouseNormY >= -1 && mouseNormY <= 1)
		{

			LOG("New ray with x = %f, y = %f", mouseNormX, mouseNormY);

			LineSegment ray = cameraComp->frustrum.UnProjectLineSegment(mouseNormX, mouseNormY);
			App->MousePick->CastRay(ray);
	/*		vec3 origin(ray.pos.x, ray.pos.y, ray.pos.z);
			LineSegment segment = ray.ToLineSegment(20);
			vec3 dest(segment.GetPoint(20).x, segment.GetPoint(20).y, segment.GetPoint(20).z);

			App->PrimManager->CreateLine(origin, dest);*/
		}
	
	}
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

float2 ModuleCamera3D::ScreenToWorld(float2 pos)
{
	float2 ret(0, 0);

	return ret;
}
// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetFustrumProjMatrix()
{
	float4x4 mat = cameraComp->frustrum.ProjectionMatrix();

	return mat.Transposed().ptr();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetFustrumMatrix()
{
	float4x4 mat = cameraComp->frustrum.ViewMatrix();
	
	return mat.Transposed().ptr();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{

	if (cameraTrans != nullptr && frustrumFollow)
	{
		cameraMat = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
		cameraMatInverse = cameraMat.Inverted();

		float3 translation, scaling;
		Quat rotation;

		cameraTrans->local_transform = cameraMat;
		cameraTrans->local_transform.Decompose(translation, rotation, scaling);
		translation.x = Position.x;
		translation.y = Position.y;
		translation.z = Position.z;
		cameraTrans->local_transform = cameraTrans->local_transform.FromTRS(translation, rotation, scaling);
		cameraTrans->local_transform.Decompose(translation, rotation, scaling);
	}

	else
	{
		ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
		ViewMatrixInverse = inverse(ViewMatrix);
	}

}
