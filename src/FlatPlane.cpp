#include "FlatPlane.h"
#include "Model.h"
#include "ModelTools.h"
#include "d3dUtil.h"

FlatPlane::FlatPlane(ID3D11Device* dev, float len, float urep, float vrep)
{
	int nverts = 4;
	StandardVertex* pVerts = new StandardVertex[nverts];

	int ntri = 2;
	TriangleByIndex* pTriList = new TriangleByIndex[ntri];

	pVerts[0].set(-0.5f * len, 0, -0.5f * len, urep, vrep,   0.f, 1.f, 0.f);
	pVerts[1].set(-0.5f * len, 0,  0.5f * len, urep, 0.f,    0.f, 1.f, 0.f);
	pVerts[2].set( 0.5f * len, 0,  0.5f * len, 0.f, 0.f,     0.f, 1.f, 0.f);
	pVerts[3].set( 0.5f * len, 0, -0.5f * len, 0.f, vrep,    0.f, 1.f, 0.f);
	
	pTriList[0].set(0, 1, 2);
	pTriList[1].set(0, 2, 3);

	ModelTools::ComputeTangent(pVerts, nverts, pTriList, ntri);

	pPlane = new Model(dev, pVerts, nverts, pTriList, ntri);

	delete[] pVerts;
	delete[] pTriList;
}

FlatPlane::~FlatPlane()
{
	delete pPlane;
}

void FlatPlane::Render(ID3D11DeviceContext* context)
{
	pPlane->SetToContext(context);
	pPlane->Render(context);
}