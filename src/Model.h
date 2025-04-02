#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>
#include "Vect.h"
#include "Align16.h"
#include "Matrix.h"

class MeshSeparator;

struct StandardVertex : public Align16
{
	Vect Pos;
	float u = 0.0f;    // UV texture coordinate
	float v = 0.0f;
	Vect normal;
	Vect Color;
	Vect tangent;
	int meshNum = 0;	   // new property

	void set(float _x, float _y, float _z,
		     float _u = 0, float _v = 0,
		     float _nx = 0, float _ny = 0, float _nz = 0,
		     float _r = 1, float _g = 1, float _b = 1, int num = 0)
	{
		this->Pos = Vect(_x, _y, _z);
		this->u = _u;
		this->v = _v;
		this->normal = Vect(_nx, _ny, _nz, 0);
		this->Color = Vect(_r, _g, _b);
		this->meshNum = num;
	}

	void set(float _x, float _y, float _z, float _u, float _v, const Vect& c, int num = 0)
	{
		this->Pos = Vect(_x, _y, _z);
		this->u = _u;
		this->v = _v;
		this->Color = c;
		this->meshNum = num;
	}

	void set(float _x, float _y, float _z, float _u, float _v, const Vect& n, const Vect& c, int num = 0)
	{
		this->Pos = Vect(_x, _y, _z);
		this->u = _u;
		this->v = _v;
		this->normal = n;
		this->Color = c;
		this->meshNum = num;
	}

	void set(float _x, float _y, float _z, const Vect& c, int num = 0)
	{
		this->Pos = Vect(_x, _y, _z);
		this->u = 0;
		this->v = 0;
		this->Color = c;
		this->meshNum = num;
	}

	void set(float _x, float _y, float _z, const Vect& n, const Vect& c, int num = 0)
	{
		this->Pos = Vect(_x, _y, _z);
		this->u = 0;
		this->v = 0;
		this->normal = n;
		this->Color = c;
		this->meshNum = num;
	}

	void set(const Vect& p, const Vect& c)
	{
		this->Pos = p;
		this->u = 0;
		this->v = 0;
		this->Color = c;
		this->meshNum = 0;
	}

	void setTangent(const Vect& t)
	{
		this->tangent = t;
	}
};

struct TriangleByIndex
{
	unsigned int v0;
	unsigned int v1;
	unsigned int v2;

	void set(int _v0, int _v1, int _v2)
	{
		v0 = _v0;
		v1 = _v1;
		v2 = _v2;
	}
};

class Model
{
private:
	// Model information
	TriangleByIndex *pTriList;
	StandardVertex* pStdVerts;
	int numVerts;
	int numTris;

	MeshSeparator* meshes;

	// GPU connections
	ID3D11Device*			mDevice;
	ID3D11Buffer*           mpVertexBuffer;
	ID3D11Buffer*           mpIndexBuffer;

	void privLoadDataToGPU();
	void privLoadDataFromFile(const char* const _modelName, StandardVertex*& pVerts,
		int& nverts, TriangleByIndex*& ptlist, int& ntri);

public:
	Model(const Model&) = delete;				// Copy constructor
	Model(Model&&) = delete;                    // Move constructor
	Model& operator=(const Model&) & = delete;  // Copy assignment operator
	Model& operator=(Model&&) & = delete;       // Move assignment operator

	enum PreMadeModels
	{
		UnitBox,
		UnitBoxRepeatedTexture,
		UnitBoxSixFaceTexture,
		UnitPyramid,
		UnitPyramidRepeatedTexture,
		UnitSphere
	};

	Model() = delete;
	Model(ID3D11Device* dev, StandardVertex *pVerts, int nverts, TriangleByIndex *ptlist, int ntri);
	Model(ID3D11Device* dev, PreMadeModels pm);
	Model(ID3D11Device* dev, const char* const _modelName);

	virtual ~Model();

	void SetToContext(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void RenderMesh(ID3D11DeviceContext* context, int meshnum);

	// Get access
	int GetMeshCount();
	bool ValidMeshNum(int i);

	int GetNumVert() const { return numVerts; }
	int GetNumTri() const { return numTris; }

	StandardVertex* GetVertArray() { return pStdVerts; }
	TriangleByIndex* GetTriArray() { return pTriList; }
};



#endif _MODEL_H_