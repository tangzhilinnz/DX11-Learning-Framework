#include "ShaderColorLight.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

ShaderColorLight::ShaderColorLight(ID3D11Device* dev, bool isToon)
	: ShaderLight(dev, (isToon
		? L"../Assets/Shaders/ColorLightToon.hlsl"
		: L"../Assets/Shaders/ColorLight.hlsl"))
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
 		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);
	ShaderLight::CreateInputLayout(layout, numElements);
}

ShaderColorLight::~ShaderColorLight()
{
}

void ShaderColorLight::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderLight::SetToContext(devcon);
}