
#include "Globals.h"
#include "Primitive.h"
#include "Application.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/include/MathGeoLib.h"


// ------------------------------------------------------------
Primitive::Primitive() : color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

Primitive::~Primitive()
{



}
// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Primitive::Update()
{
}
// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
Cube::Cube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
	to_delete = false;

}
Cube::~Cube()
{
	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);
}
Cube::Cube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
	to_delete = false;

	uint indices[36] = {
		// front
			0, 1, 2,
			2, 3, 1,
			// right
			1, 3, 5,
			3, 5, 7,
			//// back
			7, 4, 5,
			6, 7, 4,
			//// left
			6, 0, 4,
			0, 6, 2,
			//// bottom
			4, 5, 0,
			5, 1, 0,
			//// top
			6, 2, 3,
			3, 7, 6
	};

	index = new uint[36];

	for (int i = 0; i < 36; i++)
	{
		index[i] = indices[i];

	}
	num_indices = 36;

	float vertices[24] =
	{
		// front
			 0.0, 0.0,  0.0,
			 1.0 * sizeX, 0.0,  0.0,
			 0.0, 1.0 * sizeY,  0.0,
			 1.0 * sizeX, 1.0 * sizeY,  0.0,

			 // back
			  0.0, 0.0, -1.0 * sizeZ,
			  1.0 * sizeX, 0.0, -1.0 * sizeZ,
			  0.0, 1.0 * sizeY, -1.0 * sizeZ,
			  1.0 * sizeX, 1.0 * sizeY, -1.0 * sizeZ,
	};
	num_vertices = 8;

	vert = new float[24];

	for (int i = 0; i < 24; i++)
	{
		vert[i] = vertices[i];

	}
	my_indices = 0;
	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vert;
	delete[] index;

}

void Cube::Update()
{
	body.GetTransform(&transform);
}

void Cube::InnerRender() const
{
	glColor4ub(255, 255, 255, 0.0);
	if (App->UI->Wireframe_bool)
	glPushMatrix();
	/*glMultMatrixf(transform.M);*/ //This code made the cube go brrrrrrrrrrrrrrr form his initial position
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//float sx = size.x * 0.5f;
	//float sy = size.y * 0.5f;
	//float sz = size.z * 0.5f;

	//glBegin(GL_QUADS);
	//glNormal3f(0.0f, 0.0f, 1.0f);
	//glVertex3f(-sx, -sy, sz);
	//glVertex3f( sx, -sy, sz);
	//glVertex3f( sx,  sy, sz);
	//glVertex3f(-sx,  sy, sz);

	//glNormal3f(0.0f, 0.0f, -1.0f);
	//glVertex3f( sx, -sy, -sz);
	//glVertex3f(-sx, -sy, -sz);
	//glVertex3f(-sx,  sy, -sz);
	//glVertex3f( sx,  sy, -sz);

	//glNormal3f(1.0f, 0.0f, 0.0f);
	//glVertex3f(sx, -sy,  sz);
	//glVertex3f(sx, -sy, -sz);
	//glVertex3f(sx,  sy, -sz);
	//glVertex3f(sx,  sy,  sz);

	//glNormal3f(-1.0f, 0.0f, 0.0f);
	//glVertex3f(-sx, -sy, -sz);
	//glVertex3f(-sx, -sy,  sz);
	//glVertex3f(-sx,  sy,  sz);
	//glVertex3f(-sx,  sy, -sz);

	//glNormal3f(0.0f, 1.0f, 0.0f);
	//glVertex3f(-sx, sy,  sz);
	//glVertex3f( sx, sy,  sz);
	//glVertex3f( sx, sy, -sz);
	//glVertex3f(-sx, sy, -sz);

	//glNormal3f(0.0f, -1.0f, 0.0f);
	//glVertex3f(-sx, -sy, -sz);
	//glVertex3f( sx, -sy, -sz);
	//glVertex3f( sx, -sy,  sz);
	//glVertex3f(-sx, -sy,  sz);
	//glEnd();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	/*glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);


	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glPopMatrix();*/
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);

	glEnd();
	glColor3f(color.r, color.g, color.b);
}


// PYRAMID ============================================
Pyramid::Pyramid() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	to_delete = false;

	type = PrimitiveTypes::Primitive_Pyramid;
}

Pyramid::~Pyramid()
{
	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);
}

Pyramid::Pyramid(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	to_delete = false;

	type = PrimitiveTypes::Primitive_Pyramid;

	index = new uint[12];
	uint indices[12] = {
		// base
		0, 1, 2,
		//front
		0, 1, 3,
		// right
		1, 2, 3,
		//// left
		0, 2, 3,


	};
	for (int i = 0; i < 12; i++)
	{
		index[i] = indices[i];

	}
	num_indices = 12;
	vert = new float[12];
	float vertices[12] =
	{
		// base
			 0.0, 0.0,  0.0,
			 1.0 * sizeX, 0.0,  0.0,
			 0.5 * sizeX, 0,  -0.86 * sizeZ,


			 // top
				   0.5 * sizeX, 0.86 * sizeY, -0.33 * sizeZ,

	};
	num_vertices = 4;
	for (int i = 0; i < 12; i++)
	{
		vert[i] = vertices[i];

	}
	my_indices = 0;
	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vert;
	delete[] index;

}

void Pyramid::InnerRender() const
{
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// SPHERE ============================================
PrimSphere::PrimSphere() : Primitive(), radius(1.0f)
{
	to_delete = false;

	type = PrimitiveTypes::Primitive_Sphere;
}

PrimSphere::~PrimSphere()
{
	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);
}


PrimSphere::PrimSphere(float radius, unsigned int rings, unsigned int sectors) : Primitive(), radius(radius)
{
	to_delete = false;

	type = PrimitiveTypes::Primitive_Sphere;
	if (rings > 48)rings = 48;
	if (sectors > 116)sectors = 116;
	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);

	for (int r = 0; r < rings; ++r) {
		for (int s = 0; s < sectors; ++s) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			vertices.push_back(x * radius);
			vertices.push_back(y * radius);
			vertices.push_back(z * radius);

			int curRow = r * sectors;
			int nextRow = (r + 1) * sectors;
			if (s + 1 != sectors && r+1 != rings) {
				//if ((curRow + s) < rings * sectors)
					indices.push_back(curRow + s);
			//	if ((nextRow + s) < rings * sectors)
					indices.push_back(nextRow + s);
			//	if (nextRow + (s + 1) < rings * sectors)
					indices.push_back(nextRow + (s + 1));

				//if ((curRow + s) < rings * sectors )
				indices.push_back(curRow + s);
				//if (nextRow + (s + 1) < rings * sectors )
				indices.push_back(nextRow + (s + 1));
			//	if (curRow + (s + 1)  < rings * sectors )
				indices.push_back(curRow + (s + 1));
			}
		
		}
	}

	my_indices = 0;
	my_vertex = 0;
	vert = new float[vertices.size()];
	for (int i = 0; i < vertices.size(); i++)
	{
		vert[i] = vertices[i];
	}
	index = new uint[indices.size()];

	for (int i = 0; i < indices.size(); i++)
	{
		index[i] = indices[i];
	}
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vert;
	delete[] index;
}

void PrimSphere::Update()
{
	body.GetTransform(&transform);
}

void PrimSphere::InnerRender() const
{
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//// OLD// not working with buffer drawing
//	//glMatrixMode(GL_MODELVIEW);
//	//glPushMatrix();
//	//glTranslatef(0, 0, 0);
//
//	glEnableClientState(GL_VERTEX_ARRAY);
//	//16704
//	//32430
//	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
//	glDrawElements(GL_TRIANGLES, (indices.size()), GL_UNSIGNED_SHORT, &indices[0]);
//	//glPopMatrix();
//	glDisableClientState(GL_VERTEX_ARRAY);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


// CYLINDER ============================================
PrimCylinder::PrimCylinder() : Primitive(), radius(1.0f), height(1.0f), sides (30)
{
	type = PrimitiveTypes::Primitive_Cylinder;
	to_delete = false;

}

PrimCylinder::~PrimCylinder()
{
	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);
}


PrimCylinder::PrimCylinder(float radius, float height, int sides) : Primitive(), radius(radius), height(height), sides(sides)
{
	type = PrimitiveTypes::Primitive_Cylinder;
	to_delete = false;

	int n = sides;
	if (n > 100) n = 100;

	// Cylinder Bottom
	float y = -height * 0.5f;
	float x = 0;
	float z = 0;
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	y = height * 0.5f;
	x = 0;
	z = 0;
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	int start = vertices.size();
	for (int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		float const y = -height * 0.5f;
		float const x = radius * cos(a);
		float const z = radius * sin(a);
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
	}

	for (int i = start; i < vertices.size(); i+= 3 )
	{
		uint ind = i / 3;
		if (i + 3 != vertices.size())
		{
			indices.push_back(ind);
			indices.push_back(ind + 1);
			indices.push_back(0);
		}
	
	}

	start = vertices.size();
	for (int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
		float const y = height * 0.5f;
		float const x = radius * cos(a);
		float const z = radius * sin(a);

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
	}

	for (int i = start; i < vertices.size(); i += 3)
	{
		uint ind = i / 3;
		if (i + 3 != vertices.size())
		{
			indices.push_back(ind);
			indices.push_back(ind + 1);
			indices.push_back(1);
		}

	}
	int half = (vertices.size() - 6) / 2;
	int max = half + 6;
	for (int i = 6; i < max; i += 3)
	{
		uint ind = i / 3;
		if (i + 3 != max)
		{
			indices.push_back(ind);
			indices.push_back(ind + 1);
			indices.push_back(vertices.size()/3 - ind);
		}

	}

	for (int i = max; i < vertices.size(); i += 3)
	{
		uint ind = i / 3;
		if (i + 3 != vertices.size())
		{
			indices.push_back(ind);
			indices.push_back(ind + 1);
			indices.push_back(half / 3 - (ind - max/3));
		}

	}

	my_indices = 0;
	my_vertex = 0;
	vert = new float[vertices.size()];
	for (int i = 0; i < vertices.size(); i++)
	{
		vert[i] = vertices[i];
	}
	index = new uint[indices.size()];
	for (int i = 0; i < indices.size(); i++)
	{
		index[i] = indices[i];
	}
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vert;
	delete[] index;
}

void PrimCylinder::InnerRender() const
{
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	


	// Cylinder Top
	

	// Cylinder "Cover"
	/*glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);

	for (int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
		glVertex3f(-height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();
	
}

// LINE ==================================================
PrimLine::PrimLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
	to_delete = false;

}

PrimLine::~PrimLine()
{
	//glDeleteBuffers(sizeof(float) * num_vertices * 3, (GLuint*)&my_vertex);
	//glDeleteBuffers(sizeof(uint) * sizeof(uint) * num_indices, (GLuint*)&my_indices);
	
	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);

}

PrimLine::PrimLine(float x, float y, float z, float x2, float y2, float z2) : Primitive(), origin(x, y, z), destination(x2, y2, z2)
{
	type = PrimitiveTypes::Primitive_Line;
	to_delete = false;

	uint indices[2] = {
		0, 1,
	};
	index = new uint[2];
	for (int i = 0; i < 2; i++)
	{
		index[i] = indices[i];

	}
	num_indices = 2;

	float vertices[6] =
	{
		// base
			 origin.x, origin.y,  origin.z,
			 destination.x, destination.y,  destination.z,
			

	};
	num_vertices = 2;
	vert = new float[6];
	for (int i = 0; i < 6; i++)
	{
		vert[i] = vertices[i];

	}
	my_indices = 0;
	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] vert;
	delete[] index;

}

void PrimLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
	//glLineWidth(4.0f);
	//glColor4ub(255, 0.0, 0.0, 0.0);

	///*glBegin(GL_LINES);

	//glVertex3f(origin.x, origin.y, origin.z);
	//glVertex3f(destination.x, destination.y, destination.z);

	//glEnd();*/

	//glEnableClientState(GL_VERTEX_ARRAY);

	//glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	//glDrawElements(GL_LINES, num_indices, GL_UNSIGNED_INT, NULL);

	//glDisableClientState(GL_VERTEX_ARRAY);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glColor4ub(255, 255, 255, 0.0);

}

// PLANE ==================================================
PrimPlane::PrimPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	to_delete = false;
	type = PrimitiveTypes::Primitive_Plane;
}

PrimPlane::~PrimPlane()
{
	//glDeleteBuffers(sizeof(float) * vertices.size(), (GLuint*)&my_vertex);
	//glDeleteBuffers(sizeof(uint) * sizeof(uint) * indices.size(), (GLuint*)&my_indices);
	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);

}

PrimPlane::PrimPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
	to_delete = false;

	float d1 = 200;
	for (float i = -d1; i <= d1; i += 1.0f)
	{
		vertices.push_back(i);
		vertices.push_back(0.0f);
		vertices.push_back(-d1);

		vertices.push_back(i);
		vertices.push_back(0.0f);
		vertices.push_back(d1);

		vertices.push_back(-d1);
		vertices.push_back(0.0f);
		vertices.push_back(i);

		vertices.push_back(d1);
		vertices.push_back(0.0f);
		vertices.push_back(i);

	}

	int size = d1 * 8 + 4 ;
	for (int i = 0; i < size; i++)
	{
		indices.push_back(i);
	
	}

	vert = new float[vertices.size()];
	for (int i = 0; i < vertices.size(); i++)
	{
		vert[i] = vertices[i];
	}
	index = new uint[indices.size()];
	for (int i = 0; i < indices.size(); i++)
	{
		index[i] = indices[i];
	}

	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vert;
	delete[] index;
	
}

void PrimPlane::InnerRender() const
{/*
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();*/

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// NORMALS ==================================================
PrimNormals::PrimNormals() : Primitive()
{
	type = PrimitiveTypes::Primitive_Normals;
	to_delete = false;

}

PrimNormals::~PrimNormals()
{
	//glDeleteBuffers(sizeof(float) * num_vertices * 3, (GLuint*)&my_vertex);
	//glDeleteBuffers(sizeof(uint) * sizeof(uint) * num_indices, (GLuint*)&my_indices);

	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);

}

PrimNormals::PrimNormals(float* normal_array, int size) : Primitive()
{
	type = PrimitiveTypes::Primitive_Normals;
	to_delete = false;


	index = new uint[size];
	for (uint i = 0; i < size; i++)
	{
		/*index[i*2] = i;
		index[i * 2 + 1] = i + 1;*/

		index[i] = i;
	}
	num_indices = size;

	

	num_vertices = size;
	vert = normal_array;

	my_indices = 0;
	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] vert;
	delete[] index;

}

void PrimNormals::InnerRender() const
{
	glLineWidth(2.0f);
	glColor4ub(255, 0.0, 0.0, 0.0);

	/*glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();*/

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_LINES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glColor4ub(255, 255, 255, 0.0);

}

// AABB ==================================================
PrimAABB::PrimAABB() : Primitive()
{
	type = PrimitiveTypes::Primitive_AABB;
	to_delete = false;

}

PrimAABB::~PrimAABB()
{
	//glDeleteBuffers(sizeof(float) * num_vertices * 3, (GLuint*)&my_vertex);
	//glDeleteBuffers(sizeof(uint) * sizeof(uint) * num_indices, (GLuint*)&my_indices);

	glDeleteBuffers(1, &my_vertex);
	glDeleteBuffers(1, &my_indices);

}

PrimAABB::PrimAABB(AABB* bbox) : Primitive()
{
	type = PrimitiveTypes::Primitive_Normals;
	to_delete = false;

	float3 array[8];

	bbox->GetCornerPoints(array);
	vert = new float[24];
	for (int i = 0; i < 8; i++)
	{
		vert[i * 3] = array[i].x;
		vert[i * 3 + 1] = array[i].y;
		vert[i * 3 + 2] = array[i].z;
	}
	
	uint indices[24] = {
		
			0, 1,
			1, 3,
			
			3, 7,
			3, 2,
		
			0, 2,
			1, 5,
			 
			7, 6,
			2, 6,
			
			6, 4,
			4, 0,
			
			7, 5,
			4, 5,
	};

	index = new uint[24];

	for (int i = 0; i < 24; i++)
	{
		index[i] = indices[i];

	}
	num_indices = 24;



	num_vertices = 8;

	my_indices = 0;
	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vert, GL_STATIC_DRAW);
	// … bind and use other buffers

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] vert;
	delete[] index;

}

void PrimAABB::InnerRender() const
{
	glLineWidth(2.0f);
	glColor4ub(0.0, 255, 0.0, 0.0);

	/*glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();*/

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);

	glDrawElements(GL_LINES, num_indices, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glColor4ub(255, 255, 255, 0.0);

}