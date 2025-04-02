#include "ShaderNullLight.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

ShaderNullLight::ShaderNullLight(ID3D11Device* dev, WCHAR* filename)
	: ShaderBase(dev, filename)
{
	HRESULT hr = S_OK;

	// View Projection buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CamMatrices);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBufferCamMatrices);
	assert(SUCCEEDED(hr));

	// Fog buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_Fog);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBufferFog);
	assert(SUCCEEDED(hr));

	// Default initialization for essential settings
	FogPara.EyePosWorld = Vect(1.0f, 1.0f, 1.0f);
	FogPara.FogStart = FLT_MAX; // Disable fog
	FogPara.FogRange = 1.0f;    // Set to 1 (doesn't matter since FogStart is FLT_MAX)
	FogPara.FogColor = Vect(0.0f, 0.0f, 0.0f, 1.0f);
	FogPara.FogBaseHeight = 0.0f;
	FogPara.FogThickness = 0.0f;
	FogPara.ExpFogDensity = 1.0f;
	FogPara.Opacity = 1.0f;
	FogPara.FogType = (int)FogType::Undefined_Fog;
}

ShaderNullLight::~ShaderNullLight()
{
	ReleaseAndDeleteCOMobject(mpBufferCamMatrices);
	ReleaseAndDeleteCOMobject(mpBufferFog);
}

void ShaderNullLight::SendCamMatrices(const Matrix& view, const Matrix& proj)
{
	CamMatrices	mCamMatrices;
	mCamMatrices.View = view;
	mCamMatrices.Projection = proj;

	this->GetContext()->UpdateSubresource(mpBufferCamMatrices, 0, nullptr, &mCamMatrices, 0, 0);
}

void ShaderNullLight::SendFogParameters(const Vect& eyepos)
{
	FogPara.EyePosWorld = eyepos;
	this->GetContext()->UpdateSubresource(mpBufferFog, 0, nullptr, &FogPara, 0, 0);
}

void ShaderNullLight::SetFog(int fogType, const Vect& color, float start, float range,
	float thickness, float BaseHeight, float expFogDensity, float opcity)
{
	assert(start > 0.0f && range > 0.0f && thickness > 0.0f && expFogDensity > 0.0f);
	assert(fogType == 0 || fogType == 1 || fogType == 2 || fogType == 3);
	assert(opcity >= 0.0f && opcity <= 1.0f);

	FogPara.FogStart = start;
	FogPara.FogRange = range;
	FogPara.FogColor = color;
	FogPara.FogThickness = thickness;
	FogPara.FogBaseHeight = BaseHeight;
	FogPara.ExpFogDensity = expFogDensity;
	FogPara.Opacity = opcity;
	FogPara.FogType = fogType;
}


void ShaderNullLight::StartShaderPass(ID3D11DeviceContext* devcon,
	const Matrix& view, const Matrix& proj, const Vect& eyepos)
{
	this->SetToContext(devcon);
	this->SendCamMatrices(view, proj);
	this->SendFogParameters(eyepos);
}

void ShaderNullLight::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderBase::SaveContextPtr(devcon);
	ShaderBase::SetToContext_VS_PS_InputLayout();

	devcon->VSSetConstantBuffers(0, 1, &mpBufferCamMatrices);
	devcon->PSSetConstantBuffers(1, 1, &mpBufferFog);
}