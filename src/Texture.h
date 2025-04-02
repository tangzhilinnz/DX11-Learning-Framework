#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <Windows.h>
#include <string>
#include <d3d11.h>
#include "DirectXTex.h"

class TextureManager;

class Texture
{
    friend class TextureManager;

public:
	Texture(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(const Texture&) & = delete;
	Texture& operator=(Texture&&) & = delete;
	~Texture();

private:
	Texture();

public:
	void LoadTexture(
		LPCWSTR filepath,
		bool ComputeMip = false,
		size_t miplevel = 0,
		DirectX::TEX_FILTER_FLAGS filterflags = DirectX::TEX_FILTER_LINEAR);

	void LoadSampler(
		D3D11_FILTER filter,                                               // Filtering mode (e.g.,D3D11_FILTER_MIN_MAG_MIP_LINEAR)
		UINT isotropicLevel = 4,                                           // Max anisotropy level (1 for no anisotropy)
		D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP,  // Addressing mode for U coordinate
		D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP,  // Addressing mode for V coordinate
		D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP,  // Addressing mode for W coordinate
		D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER,
		FLOAT minLOD = 0,
		FLOAT maxLOD = D3D11_FLOAT32_MAX);

	void SetToContext(ID3D11DeviceContext* devcon, int texResSlot = 0, int sampSlot = 0, bool forVertex = false) const;

	void SetDevice(ID3D11Device* d3dDevice);

	void Release();
	
private:
	ID3D11Device* pmD3dDevice;
	ID3D11ShaderResourceView* mpTextureRV;
	ID3D11SamplerState* mpSampler;
};

#endif _TEXTURE_H_