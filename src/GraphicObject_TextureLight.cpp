#include "GraphicObject_TextureLight.h"
#include "Model.h"
#include <assert.h>

GraphicObject_TextureLight::GraphicObject_TextureLight(ShaderTextureLight* shader,  Model* mod)
{
	SetModel(mod );
	pShader = shader;

	mWorld = Matrix(IDENTITY);

	int n = mod->GetMeshCount();
	Textures = new Texture * [n];
	NormalMaps = new Texture * [n];
	MeshMats = new Material[n];
	for (int i = 0; i < n; i++)
	{
		Textures[i] = nullptr;
		NormalMaps[i] = nullptr;
		MeshMats[i] = { Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 1.f) };
	}
}

GraphicObject_TextureLight::~GraphicObject_TextureLight()
{
	delete[] Textures;
	delete[] NormalMaps;
	delete[] MeshMats;
}

void GraphicObject_TextureLight::SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp)
{
	for (int i = 0; i < this->pModel->GetMeshCount(); i++)
	{
		MeshMats[i].Ambient = amb;
		MeshMats[i].Diffuse = dif;
		MeshMats[i].Specular = sp;
	}
}

void GraphicObject_TextureLight::SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp, int meshnum)
{
	assert(pModel->ValidMeshNum(meshnum));

	MeshMats[meshnum].Ambient = amb;
	MeshMats[meshnum].Diffuse = dif;
	MeshMats[meshnum].Specular = sp;
}

void GraphicObject_TextureLight::SetTexture(Texture* tex)
{
	for (int i = 0; i < this->pModel->GetMeshCount(); i++)
	{
		Textures[i] = tex;
	}
}

void GraphicObject_TextureLight::SetTexture(Texture* tex, int meshnum)
{
	assert(pModel->ValidMeshNum(meshnum));
	Textures[meshnum] = tex;
}

void GraphicObject_TextureLight::SetNormalMap(Texture* tex)
{
	for (int i = 0; i < this->pModel->GetMeshCount(); i++)
	{
		NormalMaps[i] = tex;
	}
}

void GraphicObject_TextureLight::SetNormalMap(Texture* tex, int meshnum)
{
	assert(pModel->ValidMeshNum(meshnum));
	NormalMaps[meshnum] = tex;
}

void GraphicObject_TextureLight::SetWorld(const Matrix& m)
{
	mWorld = m;
}

void GraphicObject_TextureLight::Render()
{
	pModel->SetToContext(pShader->GetContext());

	for (int i = 0; i < pModel->GetMeshCount(); i++)
	{
		pShader->SendWorldAndMaterial(mWorld, MeshMats[i].Ambient, MeshMats[i].Diffuse, MeshMats[i].Specular);
		pShader->SetTextureResourceAndSampler(Textures[i], NormalMaps[i]);
		pModel->RenderMesh(pShader->GetContext(), i);
	}
}