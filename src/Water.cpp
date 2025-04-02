#include "Water.h"
#include "Model.h"
#include <assert.h>

Water::Water(ShaderWater* shader,  Model* mod)
{
	SetModel(mod );
	pShader = shader;
	pHeightMap = nullptr;

	mWorld = Matrix(IDENTITY);
	mMeshMat = { Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 1.f) };
}

Water::~Water()
{
}

void Water::SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp)
{
	mMeshMat.Ambient = amb;
	mMeshMat.Diffuse = dif;
	mMeshMat.Specular = sp;

}

void Water::SetWorld(const Matrix& m)
{ 
	mWorld = m;
}

void Water::SetHeighMapTexture(Texture* heightMap)
{
	pHeightMap = heightMap;
}

void Water::Render()
{
	pShader->SendWorldAndMaterial(mWorld, mMeshMat.Ambient, mMeshMat.Diffuse, mMeshMat.Specular);
	pShader->SetTextureResourceAndSampler(pHeightMap);
	pModel->SetToContext(pShader->GetContext());
	pModel->Render(pShader->GetContext());
}