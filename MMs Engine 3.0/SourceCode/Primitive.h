
#pragma once
#include "glmath.h"
#include "Color.h"

#include <vector>

#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysBody3D.h"

using namespace std;

typedef float GLfloat;
typedef unsigned short GLushort;


enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid,
	Primitive_Normals,
	Primitive_AABB,
};

class Primitive
{
public:

	Primitive();
	virtual ~Primitive();
	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	virtual	void	Update();

	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	bool axis,wire;
	mat4x4 transform;
	bool to_delete;
	PhysBody3D body;

protected:
	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(float sizeX = 1.f, float sizeY = 1.f, float sizeZ = 1.f);
	~Cube();
	void Update();
	void InnerRender() const;
public:
	vec3 size;
	uint my_indices = 0;
	uint my_vertex = 0;
	int num_vertices;
	int num_indices;
	float* vert;
	uint* index;

};

// ============================================
class Pyramid : public Primitive
{
public:
	Pyramid();
	~Pyramid();

	Pyramid(float sizeX = 1.f, float sizeY = 1.f, float sizeZ = 1.f);
	void InnerRender() const;
public:
	vec3 size;
	uint my_indices = 0;
	uint my_vertex = 0;
	int num_vertices;
	int num_indices;
	float* vert;
	uint* index;

};

// ============================================
class PrimSphere : public Primitive
{
public:
	PrimSphere();
	PrimSphere(float radius, unsigned int rings, unsigned int sectors);
	~PrimSphere();
	void Update();

	void InnerRender() const;
protected:
	vector<GLfloat> vertices;
	vector<GLushort> indices;
public:
	uint my_indices = 0;
	uint my_vertex = 0;
	/*float vert[16704];
	uint index[32430];*/
	float* vert;
	uint* index;
	float radius;
};

// ============================================
class PrimCylinder : public Primitive
{
public:
	PrimCylinder();
	~PrimCylinder();

	PrimCylinder(float radius, float height, int sides);
	void InnerRender() const;
protected:
	vector<GLfloat> vertices;
	vector<GLushort> indices;
	int sides;
public:
	uint my_indices = 0;
	uint my_vertex = 0;
	//float vert[732];
	//uint index[1440];
	float* vert;
	uint* index;
	float radius;
	float height;
};

// ============================================
class PrimLine : public Primitive
{
public:
	PrimLine();
	~PrimLine();

	PrimLine(float x, float y, float z, float x2, float y2, float z2);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
	int num_vertices;
	int num_indices;
	uint my_indices = 0;
	uint my_vertex = 0;
	float* vert;
	uint* index;
};

// ============================================

class PrimNormals : public Primitive
{
public:
	PrimNormals();
	~PrimNormals();

	PrimNormals(float* vert, int size);
	void InnerRender() const;
public:

	int num_vertices;
	int num_indices;
	uint my_indices = 0;
	uint my_vertex = 0;
	float* vert;
	uint* index;
};

// ============================================

class PrimAABB : public Primitive
{
public:
	PrimAABB();
	~PrimAABB();

	PrimAABB(AABB* );
	void InnerRender() const;
public:

	int num_vertices;
	int num_indices;
	uint my_indices = 0;
	uint my_vertex = 0;
	float* vert;
	uint* index;
};
// ============================================
class PrimPlane : public Primitive
{
public:
	PrimPlane();
	~PrimPlane();

	PrimPlane(float x, float y, float z, float d);
	void InnerRender() const;
protected:
	vector<GLfloat> vertices;
	vector<GLushort> indices;
public:
	uint my_indices = 0;
	uint my_vertex = 0;
	//float vert[4812];
	//uint index[1605];
	float* vert;
	uint* index;
	vec3 normal;
	float constant;
};

