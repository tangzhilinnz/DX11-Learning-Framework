#include "ShaderTexture.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>


ShaderTexture::ShaderTexture(ID3D11Device* device)
	: ShaderNullLight(device, L"../Assets/Shaders/Texture.hlsl")
{
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);
	this->CreateInputLayout(layout, numElements);

	HRESULT hr;

	// World buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_World);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBuffWorld);
	assert(SUCCEEDED(hr));
}

ShaderTexture::~ShaderTexture()
{
	ReleaseAndDeleteCOMobject(mpBuffWorld);
}

void ShaderTexture::SendWorld(const Matrix& world)
{
	Data_World wc;
	wc.World = world;
	this->GetContext()->UpdateSubresource(mpBuffWorld, 0, nullptr, &wc, 0, 0);
}

void ShaderTexture::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderNullLight::SetToContext(devcon);
	devcon->VSSetConstantBuffers(2, 1, &mpBuffWorld);
	devcon->PSSetConstantBuffers(2, 1, &mpBuffWorld);
}

void ShaderTexture::SetTextureResourceAndSampler(Texture* tex)
{
	assert(tex != nullptr);
	tex->SetToContext(this->GetContext(), 0, 0);
}


