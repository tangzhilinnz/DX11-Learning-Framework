#include "Skybox.h"
#include "Model.h"
#include <assert.h>

Skybox::Skybox(ShaderTexture* shader, Model* skyboxModel)
{
	SetModel(skyboxModel);
	pShader = shader;

	pTexture = nullptr;
	mWorld = Matrix(IDENTITY);
}

Skybox::~Skybox()
{
}

void Skybox::SetTexture(Texture* tex)
{
    pTexture = tex;
}

void Skybox::SetWorld(const Matrix& m)
{
	mWorld = m;
}

void Skybox::Render()
{
	pShader->SendWorld(mWorld);
	pShader->SetTextureResourceAndSampler(pTexture);
	pModel->SetToContext(pShader->GetContext());
	pModel->Render(pShader->GetContext());
}
