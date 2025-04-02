#include "ShaderColor.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>


ShaderColor::ShaderColor(ID3D11Device* device)
	: ShaderNullLight(device, L"../Assets/Shaders/ColorSelected3D.hlsl")
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);  
	this->CreateInputLayout(layout, numElements);

	HRESULT hr;  

	// Color buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_WorldColor);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBuffWorldColor);
	assert(SUCCEEDED(hr));
}

ShaderColor::~ShaderColor()
{
	ReleaseAndDeleteCOMobject(mpBuffWorldColor);
}

void ShaderColor::SendWorldColor(const Matrix& world, const Vect& col)
{
	Data_WorldColor wc;
	wc.World = world;
	wc.Color = col;

	this->GetContext()->UpdateSubresource(mpBuffWorldColor, 0, nullptr, &wc, 0, 0);
}

void ShaderColor::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderNullLight::SetToContext(devcon);
	devcon->VSSetConstantBuffers(2, 1, &mpBuffWorldColor);
	devcon->PSSetConstantBuffers(2, 1, &mpBuffWorldColor);
}


