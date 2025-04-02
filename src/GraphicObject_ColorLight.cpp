#include "GraphicObject_ColorLight.h"
#include "Model.h"
#include <assert.h>

GraphicObject_ColorLight::GraphicObject_ColorLight(ShaderColorLight* shader,  Model* mod)
{
	SetModel(mod );
	pShader = shader;

	mWorld = Matrix(IDENTITY);

	int n = mod->GetMeshCount();
	MeshMats = new Material[n];
	for (int i = 0; i < n; i++)
	{
		MeshMats[i] = { Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 1.f) };
	}
}

GraphicObject_ColorLight::~GraphicObject_ColorLight()
{
	delete[] MeshMats;
}

void GraphicObject_ColorLight::SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp)
{
	for (int i = 0; i < this->pModel->GetMeshCount(); i++)
	{
		MeshMats[i].Ambient = amb;
		MeshMats[i].Diffuse = dif;
		MeshMats[i].Specular = sp;
	}
}

void GraphicObject_ColorLight::SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp, int meshnum)
{
	assert(pModel->ValidMeshNum(meshnum));
	
	MeshMats[meshnum].Ambient = amb;
	MeshMats[meshnum].Diffuse = dif;
	MeshMats[meshnum].Specular = sp;
}

void GraphicObject_ColorLight::SetWorld(const Matrix& m)
{ 
	mWorld = m;
}

void GraphicObject_ColorLight::Render()
{
	pModel->SetToContext(pShader->GetContext());

	for (int i = 0; i < pModel->GetMeshCount(); i++)
	{
		pShader->SendWorldAndMaterial(mWorld, MeshMats[i].Ambient, MeshMats[i].Diffuse, MeshMats[i].Specular);
		pModel->RenderMesh(pShader->GetContext(), i);
	}
}