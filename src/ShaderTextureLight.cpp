#include "ShaderTextureLight.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

ShaderTextureLight::ShaderTextureLight(ID3D11Device* dev, WCHAR* filename)
	: ShaderLight(dev, filename)
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);
	ShaderLight::CreateInputLayout(layout, numElements);

	this->pDNM = TEX::CreateTexObj(dev);
	this->pDNM->LoadTexture(L"../Assets/Textures/DefaultNormalMap.tga");
	this->pDNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);
}

ShaderTextureLight::~ShaderTextureLight()
{
}

void ShaderTextureLight::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderLight::SetToContext(devcon);
}

void ShaderTextureLight::SetTextureResourceAndSampler(Texture* tex, Texture* normalMap)
{
	assert(tex != nullptr);
	tex->SetToContext(this->GetContext(), 0, 0);

	if (normalMap)
	{
		normalMap->SetToContext(this->GetContext(), 4, 0);
	}
	else
	{
		this->pDNM->SetToContext(this->GetContext(), 4, 0);
	}
}