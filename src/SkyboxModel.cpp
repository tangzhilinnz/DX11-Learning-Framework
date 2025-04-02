#include "SkyboxModel.h"
#include "d3dUtil.h"

SkyboxModel::SkyboxModel(ID3D11Device* dev, float scale)
{
	int nverts = 24;
	StandardVertex* pVerts = new StandardVertex[nverts];
	int ntri = 12;
	TriangleByIndex* pTriList = new TriangleByIndex[ntri];

	privCreateUnitBoxSixFacesInward(pVerts, pTriList, scale);

	pUnitBox = new Model(dev, pVerts, nverts, pTriList, ntri);

	delete[] pVerts;
	delete[] pTriList;
}

SkyboxModel::~SkyboxModel()
{
	delete pUnitBox;
}

void SkyboxModel::Render(ID3D11DeviceContext* context)
{
	pUnitBox->SetToContext(context);
	pUnitBox->Render(context);
}

void SkyboxModel::privCreateUnitBoxSixFacesInward(StandardVertex* pVerts, TriangleByIndex* pTriList, float scale)
{
	// Setting up faces
	// back
	int vind = 0;
	int tind = 0;

	pVerts[vind].set(0.5f * scale, 0.5f * scale, 0.5f * scale,        0.7501f, 0.3334f);
	pVerts[vind + 1].set(-0.5f * scale, 0.5f * scale, 0.5f * scale,   0.9999f, 0.3334f);
	pVerts[vind + 2].set(-0.5f * scale, -0.5f * scale, 0.5f * scale,  0.9999f, 0.6665f);
	pVerts[vind + 3].set(0.5f * scale, -0.5f * scale, 0.5f * scale,   0.7501f, 0.6665f);
	pTriList[tind].set(vind +2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// forward
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f * scale, 0.5f * scale, -0.5f * scale,       0.4999f, 0.3334f);
	pVerts[vind + 1].set(-0.5f * scale, 0.5f * scale, -0.5f * scale,  0.2501f, 0.3334f);
	pVerts[vind + 2].set(-0.5f * scale, -0.5f * scale, -0.5f * scale, 0.2501f, 0.6665f);
	pVerts[vind + 3].set(0.5f * scale, -0.5f * scale, -0.5f * scale,  0.4999f, 0.6665f);
	pTriList[tind].set(vind, vind + 1, vind + 2);
	pTriList[tind + 1].set(vind, vind + 2, vind + 3);

	// right
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f * scale, 0.5f * scale, -0.5f * scale,      0.5001f, 0.3334f);
	pVerts[vind + 1].set(0.5f * scale, 0.5f * scale, 0.5f * scale,   0.7499f, 0.3334f);
	pVerts[vind + 2].set(0.5f * scale, -0.5f * scale, 0.5f * scale,  0.7499f, 0.6665f);
	pVerts[vind + 3].set(0.5f * scale, -0.5f * scale, -0.5f * scale, 0.5001f, 0.6665f);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// left
	vind += 4;
	tind += 2;
	pVerts[vind].set(-0.5f * scale, 0.5f * scale, 0.5f * scale,       0.0001f, 0.3334f);
	pVerts[vind + 1].set(-0.5f * scale, 0.5f * scale, -0.5f * scale,  0.2499f, 0.3334f);
	pVerts[vind + 2].set(-0.5f * scale, -0.5f * scale, -0.5f * scale, 0.2499f, 0.6665f);
	pVerts[vind + 3].set(-0.5f * scale, -0.5f * scale, 0.5f * scale, 0.0001f, 0.6665f);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// Top
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f * scale, 0.5f * scale, -0.5f * scale,       0.4999f, 0.3332f);
	pVerts[vind + 1].set(-0.5f * scale, 0.5f * scale, -0.5f * scale,  0.2501f, 0.3332f);
	pVerts[vind + 2].set(-0.5f * scale, 0.5f * scale, 0.5f * scale,   0.2501f, 0.0001f);
	pVerts[vind + 3].set(0.5f * scale, 0.5f * scale, 0.5f * scale,    0.4999f, 0.0001f);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);

	// Bottom
	vind += 4;
	tind += 2;
	pVerts[vind].set(0.5f * scale, -0.5f * scale, 0.5f * scale,        0.4999f, 0.9999f);
	pVerts[vind + 1].set(-0.5f * scale, -0.5f * scale, 0.5f * scale,   0.2501f, 0.9999f);
	pVerts[vind + 2].set(-0.5f * scale, -0.5f * scale, -0.5f * scale,  0.2501f, 0.6667f);
	pVerts[vind + 3].set(0.5f * scale, -0.5f * scale, -0.5f * scale,   0.4999f, 0.6667f);
	pTriList[tind].set(vind + 2, vind + 1, vind);
	pTriList[tind + 1].set(vind + 3, vind + 2, vind);
}