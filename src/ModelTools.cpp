#include "ModelTools.h"
//#include "Enum.h"
#include "Matrix.h"
#include <assert.h>
#include "Model.h"
#include "d3dUtil.h"

// Enable additional constants M_PI
#define _USE_MATH_DEFINES
#include <math.h>


/// Creates the unit box centered at the origin
void ModelTools::CreateUnitBox(StandardVertex *&pVerts, int& nverts, TriangleByIndex *&pTriList, int& ntri)
{
	nverts = 24;
	pVerts = new StandardVertex[nverts];
	ntri = 12;
	pTriList = new TriangleByIndex[ntri];


	// Setting up faces
	// Forward
	int vind = 0;
	int tind = 0;
	pVerts[vind].set(0.5f, 0.5f, 0.5f,        Vect(0.f, 0.f, 1.f, 0.0f), Colors::Black);
	pVerts[vind + 1].set(-0.5f, 0.5f, 0.5f,   Vect(0.f, 0.f, 1.f, 0.0f), Colors::Black);
	pVerts[vind + 2].set(-0.5f, -0.5f, 0.5f,  Vect(0.f, 0.f, 1.f, 0.0f), Colors::Black);
	pVerts[vind + 3].set(0.5f, -0.5f, 0.5f,   Vect(0.f, 0.f, 1.f, 0.0f), Colors::Black);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Back
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f,       Vect(0.f, 0.f, -1.f, 0.0f), Colors::Lime);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f,  Vect(0.f, 0.f, -1.f, 0.0f), Colors::Lime);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, Vect(0.f, 0.f, -1.f, 0.0f), Colors::Lime);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f,  Vect(0.f, 0.f, -1.f, 0.0f), Colors::Lime);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// Right
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f,       Vect(1.f, 0.f, 0.f, 0.0f), Colors::Yellow);
	pVerts[vind + 1].set(0.5f, 0.5f, 0.5f,    Vect(1.f, 0.f, 0.f, 0.0f), Colors::Yellow);
	pVerts[vind + 2].set(0.5f, -0.5f, 0.5f,   Vect(1.f, 0.f, 0.f, 0.0f), Colors::Yellow);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f,  Vect(1.f, 0.f, 0.f, 0.0f), Colors::Yellow);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Left
	vind += 4;
	tind += 2;
	pVerts[vind].set(-0.5f, 0.5f, 0.5f,       Vect(-1.f, 0.f, 0.f, 0.0f), Colors::Red);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f,  Vect(-1.f, 0.f, 0.f, 0.0f), Colors::Red);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, Vect(-1.f, 0.f, 0.f, 0.0f), Colors::Red);
	pVerts[vind + 3].set(-0.5f, -0.5f, 0.5f,  Vect(-1.f, 0.f, 0.f, 0.0f), Colors::Red);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Top
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f,       Vect(0.f, 1.f, 0.f, 0.0f), Colors::Blue);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f,  Vect(0.f, 1.f, 0.f, 0.0f), Colors::Blue);
	pVerts[vind + 2].set(-0.5f, 0.5f, 0.5f,   Vect(0.f, 1.f, 0.f, 0.0f), Colors::Blue);
	pVerts[vind + 3].set(0.5f, 0.5f, 0.5f,    Vect(0.f, 1.f, 0.f, 0.0f), Colors::Blue);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Bottom
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, -0.5f, 0.5f,       Vect(0.f, -1.f, 0.f, 0.0f), Colors::Cyan);
	pVerts[vind + 1].set(-0.5f, -0.5f, 0.5f,  Vect(0.f, -1.f, 0.f, 0.0f), Colors::Cyan);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, Vect(0.f, -1.f, 0.f, 0.0f), Colors::Cyan);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f,  Vect(0.f, -1.f, 0.f, 0.0f), Colors::Cyan);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);
}

void ModelTools::CreateUnitBoxRepTexture(StandardVertex*& pVerts, int& nverts, TriangleByIndex*& pTriList, int& ntri)
{
	nverts = 24;
	pVerts = new StandardVertex[nverts];
	ntri = 12;
	pTriList = new TriangleByIndex[ntri];


	// Setting up faces
	// Forward
	int vind = 0;
	int tind = 0;
	pVerts[vind].set(0.5f, 0.5f, 0.5f, 1.f, 0.f,          0.f, 0.f, 1.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, 0.5f, 0.f, 0.f,     0.f, 0.f, 1.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, 0.5f, 0.f, 1.f,    0.f, 0.f, 1.f);
	pVerts[vind + 3].set(0.5f, -0.5f, 0.5f, 1.f, 1.f,     0.f, 0.f, 1.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Back
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f, 0.f, 0.f,         0.f, 0.f, -1.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f, 1.f, 0.f,    0.f, 0.f, -1.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, 1.f, 1.f,   0.f, 0.f, -1.f);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f, 0.f, 1.f,    0.f, 0.f, -1.f);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// Right
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f, 1, 0.f,           1.f, 0.f, 0.f);
	pVerts[vind + 1].set(0.5f, 0.5f, 0.5f, 0.f, 0.f,      1.f, 0.f, 0.f);
	pVerts[vind + 2].set(0.5f, -0.5f, 0.5f, 0.f, 1.f,     1.f, 0.f, 0.f);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f, 1.f, 1.f,    1.f, 0.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Left
	vind += 4;
	tind += 2;
	pVerts[vind].set(-0.5f, 0.5f, 0.5f, 1.f, 0.f,         -1.f, 0.f, 0.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f, 0.f, 0.f,    -1.f, 0.f, 0.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, 0.f, 1.f,   -1.f, 0.f, 0.f);
	pVerts[vind + 3].set(-0.5f, -0.5f, 0.5f, 1.f, 1.f,    -1.f, 0.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Top
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f, 1.f, 0.f,          0.f, 1.f, 0.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f, 0.f, 0.f,     0.f, 1.f, 0.f);
	pVerts[vind + 2].set(-0.5f, 0.5f, 0.5f, 0.f, 1.f,      0.f, 1.f, 0.f);
	pVerts[vind + 3].set(0.5f, 0.5f, 0.5f, 1.f, 1.f,       0.f, 1.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Bottom
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, -0.5f, 0.5f, 1.f, 0.f,          0.f, -1.f, 0.f);
	pVerts[vind + 1].set(-0.5f, -0.5f, 0.5f, 0.f, 0.f,     0.f, -1.f, 0.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, 0.f, 1.f,    0.f, -1.f, 0.f);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f, 1.f, 1.f,     0.f, -1.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	ModelTools::ComputeTangent(pVerts, nverts, pTriList, ntri);
}

void ModelTools::CreateUnitBoxSixFaceTexture(StandardVertex*& pVerts, int& nverts, TriangleByIndex*& pTriList, int& ntri)
{
	nverts = 24;
	pVerts = new StandardVertex[nverts];
	ntri = 12;
	pTriList = new TriangleByIndex[ntri];

	// Setting up faces
	// Forward
	int vind = 0;
	int tind = 0;
	pVerts[vind].set(0.5f, 0.5f, 0.5f, 0.25f, 0.625f,         0.f, 0.f, 1.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, 0.5f, 0.25f, 0.375f,    0.f, 0.f, 1.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, 0.5f, 0.0f, 0.375f,    0.f, 0.f, 1.f);
	pVerts[vind + 3].set(0.5f, -0.5f, 0.5f, 0.0f, 0.625f,     0.f, 0.f, 1.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Back
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f, 0.5f, 0.625f,         0.f, 0.f, -1.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f, 0.5f, 0.375f,    0.f, 0.f, -1.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, 0.75f, 0.375f,  0.f, 0.f, -1.f);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f, 0.75f, 0.625f,   0.f, 0.f, -1.f);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// Right
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f, 0.5f, 0.625f,         1.f, 0.f, 0.f);
	pVerts[vind + 1].set(0.5f, 0.5f, 0.5f, 0.25f, 0.625f,     1.f, 0.f, 0.f);
	pVerts[vind + 2].set(0.5f, -0.5f, 0.5f, 0.25f, 0.875f,    1.f, 0.f, 0.f);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f, 0.5f, 0.875f,    1.f, 0.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Left
	vind += 4;
	tind += 2;
	pVerts[vind].set(-0.5f, 0.5f, 0.5f, 0.25f, 0.375f,       -1.f, 0.f, 0.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f, 0.50f, 0.375f,  -1.f, 0.f, 0.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, 0.50f, 0.125f, -1.f, 0.f, 0.f);
	pVerts[vind + 3].set(-0.5f, -0.5f, 0.5f, 0.25f, 0.125f,  -1.f, 0.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Top
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, 0.5f, -0.5f, 0.50f, 0.625f,       0.f, 1.f, 0.f);
	pVerts[vind + 1].set(-0.5f, 0.5f, -0.5f, 0.50f, 0.375f,  0.f, 1.f, 0.f);
	pVerts[vind + 2].set(-0.5f, 0.5f, 0.5f, 0.25f, 0.375f,   0.f, 1.f, 0.f);
	pVerts[vind + 3].set(0.5f, 0.5f, 0.5f, 0.25f, 0.625f,    0.f, 1.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// Bottom
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f, -0.5f, 0.5f, 1.0f, 0.625f,        0.f, -1.f, 0.f);
	pVerts[vind + 1].set(-0.5f, -0.5f, 0.5f, 1.0f, 0.375f,   0.f, -1.f, 0.f);
	pVerts[vind + 2].set(-0.5f, -0.5f, -0.5f, 0.75f, 0.375f, 0.f, -1.f, 0.f);
	pVerts[vind + 3].set(0.5f, -0.5f, -0.5f, 0.75f, 0.625f,  0.f, -1.f, 0.f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	ModelTools::ComputeTangent(pVerts, nverts, pTriList, ntri);
}

/// Creates the unit 4-sided pyramid centered at the origin
void ModelTools::CreateUnitPyramid(StandardVertex*& pVerts, int& nverts, TriangleByIndex*& pTriList, int& ntri)
{
	nverts = 16;
	pVerts = new StandardVertex[nverts];
	ntri = 6;
	pTriList = new TriangleByIndex[ntri];

	pVerts[0].set(0.0f, 0.5f, 0.0f,     Colors::White);
	pVerts[1].set(-0.5f, -0.5f, -0.5f,  Colors::White);
	pVerts[2].set(0.5f, -0.5f, -0.5f,   Colors::White);
	pTriList[0].set(0, 2, 1); // Back face

	pVerts[3].set(0.0f, 0.5f, 0.0f,     Colors::White);
	pVerts[4].set(0.5f, -0.5f, -0.5f,   Colors::White);
	pVerts[5].set(0.5f, -0.5f, 0.5f,    Colors::White);
	pTriList[1].set(3, 5, 4); // Right face

	pVerts[6].set(0.0f, 0.5f, 0.0f,     Colors::White);
	pVerts[7].set(0.5f, -0.5f, 0.5f,    Colors::White);
	pVerts[8].set(-0.5f, -0.5f, 0.5f,   Colors::White);
	pTriList[2].set(6, 8, 7); // Front face

	pVerts[9].set(0.0f, 0.5f, 0.0f,     Colors::White);
	pVerts[10].set(-0.5f, -0.5f, 0.5f,  Colors::White);
	pVerts[11].set(-0.5f, -0.5f, -0.5f, Colors::White);
	pTriList[3].set(9, 11, 10); // Left face

	// For each triangle in the mesh:
	for (size_t i = 0; i < 4; i++)
	{
		// Indices of the ith triangle
		unsigned int i0 = pTriList[i].v0;
		unsigned int i1 = pTriList[i].v1;
		unsigned int i2 = pTriList[i].v2;

		// Compute edge vectors
		Vect e0 = pVerts[i1].Pos - pVerts[i0].Pos;
		Vect e1 = pVerts[i2].Pos - pVerts[i0].Pos;

		// Compute face normal
		Vect faceNormal = e0.cross(e1);
		faceNormal.norm();
		faceNormal.W() = 0.0f;

		pVerts[i0].normal = faceNormal;
		pVerts[i1].normal = faceNormal;
		pVerts[i2].normal = faceNormal;
	}

	Vect baseNoraml(0.0f, -1.0f, 0.0f, 0.0f);

	// Base face
	pVerts[12].set(-0.5f, -0.5f, -0.5f, baseNoraml, Colors::White);
	pVerts[13].set(0.5f, -0.5f, -0.5f,  baseNoraml, Colors::White);
	pVerts[14].set(0.5f, -0.5f, 0.5f,   baseNoraml, Colors::White);
	pVerts[15].set(-0.5f, -0.5f, 0.5f,  baseNoraml, Colors::White);
	pTriList[4].set(12, 13, 15);
	pTriList[5].set(13, 14, 15);

}

// Creates the unit pyramid RepTexture with uv coordinate for repetitive texture on all 5 faces
void ModelTools::CreateUnitPyramidRepTexture(StandardVertex*& pVerts, int& nverts, TriangleByIndex*& pTriList, int& ntri)
{
	nverts = 16;
	pVerts = new StandardVertex[nverts];
	ntri = 6;
	pTriList = new TriangleByIndex[ntri];

	pVerts[0].set(0.0f, 0.5f, 0.0f, 0.5f, 0.0f); 
	pVerts[1].set(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f);
	pVerts[2].set(0.5f, -0.5f, -0.5f, 0.0f, 1.0f);
	pTriList[0].set(0, 2, 1); // Back face

	pVerts[3].set(0.0f, 0.5f, 0.0f, 0.5f, 0.0f);
	pVerts[4].set(0.5f, -0.5f, -0.5f, 1.0f, 1.0f);
	pVerts[5].set(0.5f, -0.5f, 0.5f, 0.0f, 1.0f);
	pTriList[1].set(3, 5, 4); // Right face

	pVerts[6].set(0.0f, 0.5f, 0.0f, 0.5f, 0.0f);
	pVerts[7].set(0.5f, -0.5f, 0.5f, 1.0f, 1.0f);
	pVerts[8].set(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f);
	pTriList[2].set(6, 8, 7); // Front face

	pVerts[9].set(0.0f, 0.5f, 0.0f, 0.5f, 0.0f);
	pVerts[10].set(-0.5f, -0.5f, 0.5f, 1.0f, 1.0f);
	pVerts[11].set(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f);
	pTriList[3].set(9, 11, 10); // Left face

	// For each triangle in the mesh:
	for (size_t i = 0; i < 4; i++)
	{
		// Indices of the ith triangle
		unsigned int i0 = pTriList[i].v0;
		unsigned int i1 = pTriList[i].v1;
		unsigned int i2 = pTriList[i].v2;

		// Compute edge vectors
		Vect e0 = pVerts[i1].Pos - pVerts[i0].Pos;
		Vect e1 = pVerts[i2].Pos - pVerts[i0].Pos;

		// Compute face normal
		Vect faceNormal = e0.cross(e1);
		faceNormal.norm();
		faceNormal.W() = 0.0f;

		pVerts[i0].normal = faceNormal;
		pVerts[i1].normal = faceNormal;
		pVerts[i2].normal = faceNormal;
	}

	// Base face
	pVerts[12].set(-0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f);
	pVerts[13].set(0.5f, -0.5f, -0.5f,    1.0f, 1.0f,   0.0f, -1.0f, 0.0f);
	pVerts[14].set(0.5f, -0.5f, 0.5f,     1.0f, 0.0f,   0.0f, -1.0f, 0.0f);
	pVerts[15].set(-0.5f, -0.5f, 0.5f,    0.0f, 0.0f,   0.0f, -1.0f, 0.0f);
	pTriList[4].set(12, 13, 15);
	pTriList[5].set(13, 14, 15);

	ModelTools::ComputeTangent(pVerts, nverts, pTriList, ntri);
}

/// Creates the unit sphere centered at the origin
void ModelTools::CreateUnitSphere(StandardVertex*& pVerts, int& nverts, TriangleByIndex*& pTriList, int& ntri, int vslice, int hslice)
{
	// Ensure valid slice parameters
	if (vslice < 2 || hslice < 3)
	{
		assert(false);
	}

	nverts = (vslice + 1) * (hslice + 1);
	ntri = vslice * hslice * 2;

	pVerts = new StandardVertex[nverts];
	pTriList = new TriangleByIndex[ntri];

	int vertIndex = 0;
	for (int i = 0; i <= vslice; i++)
	{
		for (int j = 0; j <= hslice; j++)
		{
			float phi = i * (float)M_PI / vslice;
			float theta = j * 2 * (float)M_PI / hslice;

			float y = cosf(phi);
			float x = sinf(phi) * cosf(theta);
			float z = sinf(phi) * sinf(theta);

			float u = (float)j / (float)hslice;
			float v = (float)i / (float)vslice;

			Vect normal = Vect(x, y, z, 0.0f);
			normal.norm();
			normal.W() = 0.0f;

			pVerts[vertIndex++].set(x, y, z, u, v, normal, Colors::White);
		}
	}

	int triIndex = 0;
	for (int i = 0; i < vslice; i++)
	{
		for (int j = 0; j < hslice; j++)
		{
			// The total number of intersection points between all longitudes and a single
			// arbitrary latitude is hslice + 1
			int p1 = i * (hslice + 1) + j;
			int p2 = p1 + hslice + 1;
			int p3 = p1 + 1;
			int p4 = p2 + 1;

			pTriList[triIndex++].set(p1, p3, p2);
			pTriList[triIndex++].set(p3, p4, p2);
		}
	}

	ModelTools::ComputeTangent(pVerts, nverts, pTriList, ntri, true);
}

void ModelTools::ComputeTangent(StandardVertex*& pVerts, int& nverts, TriangleByIndex*& pTriList, int& ntri, bool continuous)
{
	for (int i = 0; i < nverts; i++)
	{
		pVerts[i].tangent = Vect(0.0f, 0.0f, 0.0f, 0.0f);
	}

	for (int i = 0; i < ntri; i++)
	{
		// 1 Retrieve the indices of the three vertices of the current triangle
		int idx0 = pTriList[i].v0;
		int idx1 = pTriList[i].v1;
		int idx2 = pTriList[i].v2;

		//2 Use the vertex indices to find the position vectors
		Vect p0 = pVerts[idx0].Pos;
		Vect p1 = pVerts[idx1].Pos;
		Vect p2 = pVerts[idx2].Pos;

		//3 Use the vertex indices to find the UV texture coordinates
		float u0 = pVerts[idx0].u;
		float v0 = pVerts[idx0].v;
		float u1 = pVerts[idx1].u;
		float v1 = pVerts[idx1].v;
		float u2 = pVerts[idx2].u;
		float v2 = pVerts[idx2].v;

		//4 Compute the tangent for the current triangle
		Vect e0 = p1 - p0;
		Vect e1 = p2 - p1;

		float du0 = u1 - u0;
		float du1 = u2 - u1;
		float dv0 = v1 - v0;
		float dv1 = v2 - v1;

		float f = 1.0f / (du0 * dv1 - du1 * dv0);

		Vect tangent(
			f * (dv1 * e0.X() - dv0 * e1.X()),
			f * (dv1 * e0.Y() - dv0 * e1.Y()),
			f * (dv1 * e0.Z() - dv0 * e1.Z()),
			0.0f);
		tangent.norm();
		tangent.W() = 0.0f;

		//5 Orthogonalize the tangent with respect to the normal for each vertex
		// (producing three different tangents)
		Vect normal0 = pVerts[idx0].normal;
		Vect normal1 = pVerts[idx1].normal;
		Vect normal2 = pVerts[idx2].normal;

		Vect tangent0 = tangent - tangent.dot(normal0) * normal0;
		Vect tangent1 = tangent - tangent.dot(normal1) * normal1;
		Vect tangent2 = tangent - tangent.dot(normal2) * normal2;
		tangent0.W() = 0.0f;
		tangent1.W() = 0.0f;
		tangent2.W() = 0.0f;

		//6 Accumulate the computed tangents for each vertex

		if (continuous)
		{
			Vect faceNormal = e0.cross(e1);
			faceNormal.W() = 0.0f;
			float area = faceNormal.mag() * 0.5f; // Triangle area

			pVerts[idx0].tangent += tangent0 * area;
			pVerts[idx1].tangent += tangent1 * area;
			pVerts[idx2].tangent += tangent2 * area;
		}
		else
		{
			pVerts[idx0].tangent = tangent0;
			pVerts[idx1].tangent = tangent1;
			pVerts[idx2].tangent = tangent2;
		}
	}

	//7 Normalize the final accumulated tangents
	for (int i = 0; i < nverts; i++)
	{
		pVerts[i].tangent.norm();
		pVerts[i].tangent.W() = 0.0f;
	}
}
