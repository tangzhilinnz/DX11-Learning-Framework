#include "Terrain.h"
#include "Model.h"
#include <assert.h>

Terrain::Terrain(ShaderTerrain* shader,  Model* terrainMod)
{
	SetModel(terrainMod);
	pShader = shader;

	mWorld = Matrix(IDENTITY);

	assert(terrainMod->GetMeshCount() == 1);
	MeshMats = { Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 1.f) };
	for (int i = 0; i < 2; i++)
	{
		Textures[i] = nullptr;
		NormalMaps[i] = nullptr;
	}
}

Terrain::~Terrain()
{
}

void Terrain::SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp)
{
	MeshMats.Ambient = amb;
	MeshMats.Diffuse = dif;
	MeshMats.Specular = sp;
}

void Terrain::SetTexture(Texture* tex)
{
    Textures[0] = tex;
	Textures[1] = tex;
}

void Terrain::SetTexture(Texture* tex, int meshnum)
{
	assert(meshnum == 0 || meshnum == 1);
	Textures[meshnum] = tex;
}

void Terrain::SetNormalMap(Texture* tex)
{
    NormalMaps[0] = tex;
	NormalMaps[1] = tex;
}

void Terrain::SetNormalMap(Texture* tex, int meshnum)
{
	assert(meshnum == 0 || meshnum == 1);
	NormalMaps[meshnum] = tex;
}

void Terrain::SetWorld(const Matrix& m)
{
	mWorld = m;
}

void Terrain::Render()
{
	pShader->SetTextureResourceAndSampler(Textures[0], Textures[1], NormalMaps[0], NormalMaps[1]);
	pShader->SendWorldAndMaterial(mWorld, MeshMats.Ambient, MeshMats.Diffuse, MeshMats.Specular);
	pModel->SetToContext(pShader->GetContext());
	pModel->Render(pShader->GetContext());
}