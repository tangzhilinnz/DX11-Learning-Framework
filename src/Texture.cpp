#include "Texture.h"
#include "d3dUtil.h"
#include "DirectXTex.h"
#include <d3d11.h>
#include <assert.h>
#include "TextureManager.h"

Texture::~Texture()
{
	//TextureManager are responsible for managing and releasing resources.
	//ReleaseAndDeleteCOMobject(mpTextureRV);
	//ReleaseAndDeleteCOMobject(mpSampler);

	OutputDebugStringW(L"---- ~Texture() ----\n");
}

Texture::Texture()
	: pmD3dDevice(nullptr),
	mpTextureRV(nullptr),
	mpSampler(nullptr)
{
}

void Texture::LoadTexture(LPCWSTR filepath, bool ComputeMip, size_t miplevel,
	DirectX::TEX_FILTER_FLAGS filterflags)
{
	mpTextureRV = TextureManager::LoadTexture(pmD3dDevice, filepath, ComputeMip,
		miplevel, filterflags);
}

void Texture::LoadSampler(
	D3D11_FILTER filter,
	UINT isotropicLevel,
	D3D11_TEXTURE_ADDRESS_MODE addressU,
	D3D11_TEXTURE_ADDRESS_MODE addressV,
	D3D11_TEXTURE_ADDRESS_MODE addressW,
	D3D11_COMPARISON_FUNC comparisonFunc,
	FLOAT minLOD,
	FLOAT maxLOD)
{
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = filter;
	sampDesc.MaxAnisotropy = isotropicLevel;
	sampDesc.AddressU = addressU;
	sampDesc.AddressV = addressV;
	sampDesc.AddressW = addressW;
	sampDesc.ComparisonFunc = comparisonFunc;
	sampDesc.MinLOD = minLOD;
	sampDesc.MaxLOD = maxLOD;

	mpSampler = TextureManager::LoadSampler(pmD3dDevice, sampDesc);
}

void Texture::SetToContext(ID3D11DeviceContext* devcon, int texResSlot,  int sampSlot, bool forVertex) const
{
	assert(mpTextureRV != nullptr);
	assert(mpSampler != nullptr);

	if (forVertex)
	{
		devcon->VSSetShaderResources(texResSlot, 1, &mpTextureRV);
		devcon->VSSetSamplers(sampSlot, 1, &mpSampler);
	}

	devcon->PSSetShaderResources(texResSlot, 1, &mpTextureRV);
	devcon->PSSetSamplers(sampSlot, 1, &mpSampler);
}

void Texture::SetDevice(ID3D11Device* d3dDevice)
{
	pmD3dDevice = d3dDevice;
}

void Texture::Release()
{
	pmD3dDevice = nullptr;
	mpTextureRV = nullptr;
	mpSampler = nullptr;
}