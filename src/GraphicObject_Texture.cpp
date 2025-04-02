#include "GraphicObject_Texture.h"
#include "Model.h"
#include <assert.h>

GraphicObject_Texture::GraphicObject_Texture(ShaderTexture* shader, Model* mod)
{
	SetModel(mod );
	pShader = shader;

	//pTexture = nullptr;
	World = Matrix(IDENTITY);

	int n = mod->GetMeshCount();
	Textures = new Texture*[n];
	for (int i = 0; i < n; i++)
	{
		Textures[i] = nullptr;
	}
}

GraphicObject_Texture::~GraphicObject_Texture()
{
	delete[] Textures;
}

void GraphicObject_Texture::SetTexture(Texture* tex)
{
	for (int i = 0; i < this->pModel->GetMeshCount(); i++)
	{
		Textures[i] = tex;
	}
}

void GraphicObject_Texture::SetTexture(Texture* tex, int meshnum)
{
	assert(pModel->ValidMeshNum(meshnum));
	Textures[meshnum] = tex;
}

void GraphicObject_Texture::SetWorld(const Matrix& m)
{ 
	World = m;
}

void GraphicObject_Texture::Render()
{
	pModel->SetToContext(pShader->GetContext());
	pShader->SendWorld(World);

	for (int i = 0; i < pModel->GetMeshCount(); i++)
	{
		pShader->SetTextureResourceAndSampler(Textures[i]);
		pModel->RenderMesh(pShader->GetContext(), i);
	}
}