#include "ShaderTerrain.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

ShaderTerrain::ShaderTerrain(ID3D11Device* dev)
	: ShaderLight(dev, L"../Assets/Shaders/TerrainModel.hlsl")
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
	this->CreateInputLayout(layout, numElements);

	this->pDNM = TEX::CreateTexObj(dev);
	this->pDNM->LoadTexture(L"../Assets/Textures/DefaultNormalMap.tga");
	this->pDNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);
}

ShaderTerrain::~ShaderTerrain()
{
}

void ShaderTerrain::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderLight::SetToContext(devcon);
}

//void ShaderTerrain::SetTextureResourceAndSampler(Texture* tex, Texture* tex2, Texture* tex3)
//{
//	assert(tex && tex2 && tex3);
//
//	tex->SetToContext(this->GetContext(), 0, 0);
//	tex2->SetToContext(this->GetContext(), 3, 1);
//	tex3->SetToContext(this->GetContext(), 4, 2);
//}

void ShaderTerrain::SetTextureResourceAndSampler(Texture* tex1, Texture* tex2,
	Texture* normalMap1, Texture* normalMap2)
{
	assert(tex1 && tex2);
	tex1->SetToContext(this->GetContext(), 0, 0);
	tex2->SetToContext(this->GetContext(), 3, 1);

	if (normalMap1)
	{
		normalMap1->SetToContext(this->GetContext(), 4, 0);
	}
	else
	{
		this->pDNM->SetToContext(this->GetContext(), 4, 0);
	}

	if (normalMap2)
	{
		normalMap2->SetToContext(this->GetContext(), 5, 1);
	}
	else
	{
		this->pDNM->SetToContext(this->GetContext(), 5, 1);
	}
}