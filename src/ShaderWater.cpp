#include "ShaderWater.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

ShaderWater::ShaderWater(ID3D11Device* dev)
	: ShaderLight(dev, L"../Assets/Shaders/WaterModel.hlsl")
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
 		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);
	ShaderLight::CreateInputLayout(layout, numElements);

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;

	// Water buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_Water);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBufferWater);
	assert(SUCCEEDED(hr));


	// Default initialization for essential settings
	WaterPara.time = 0.0f;
	WaterPara.waterSizeY = 0.0f;
	WaterPara.heightMapSize = 0.0f;
}

ShaderWater::~ShaderWater()
{
	ReleaseAndDeleteCOMobject(mpBufferWater);
}

void ShaderWater::SetWater(float time, float heightMapSize, float waterSizeY)
{
	WaterPara.time = time;
	WaterPara.heightMapSize = heightMapSize;
	WaterPara.waterSizeY = waterSizeY;
}

void ShaderWater::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderLight::SetToContext(devcon);
	devcon->VSSetConstantBuffers(4, 1, &mpBufferWater);
	devcon->PSSetConstantBuffers(4, 1, &mpBufferWater);
}

void ShaderWater::SetTextureResourceAndSampler(Texture* heightMap)
{
	assert(heightMap != nullptr);
	heightMap->SetToContext(this->GetContext(), 0, 0, true);
}

void ShaderWater::SendWaterParameters()
{
	this->GetContext()->UpdateSubresource(mpBufferWater, 0, nullptr, &WaterPara, 0, 0);
}

void ShaderWater::StartShaderPass(ID3D11DeviceContext* devcon, const Matrix& view,
	const Matrix& proj, const Vect& eyepos)
{
	ShaderLight::StartShaderPass(devcon, view, proj, eyepos);
	this->SendWaterParameters();
}