#include "ShaderColorOptimized.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>


ShaderColorOptimized::ShaderColorOptimized(ID3D11Device* device)
	: ShaderBase(device, L"../Assets/Shaders/ColorSelected3DOptimized.hlsl")
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);
	this->CreateInputLayout(layout, numElements);

	HRESULT hr;

	// WVP and Color buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_WVPColor);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBuffWVPColor);
	assert(SUCCEEDED(hr));

}

ShaderColorOptimized::~ShaderColorOptimized()
{
	ReleaseAndDeleteCOMobject(mpBuffWVPColor);
}

void ShaderColorOptimized::SendWVPColor(const Matrix& WVP, const Vect& col)
{
	Data_WVPColor mWVPColor;
	mWVPColor.WVP = WVP;
	mWVPColor.Color = col;

	this->GetContext()->UpdateSubresource(mpBuffWVPColor, 0, nullptr, &mWVPColor, 0, 0);
}

void ShaderColorOptimized::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderBase::SaveContextPtr(devcon);
	ShaderBase::SetToContext_VS_PS_InputLayout();

	devcon->VSSetConstantBuffers(0, 1, &mpBuffWVPColor);
}