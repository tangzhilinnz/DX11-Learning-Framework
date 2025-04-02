#include "ShaderLight.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

ShaderLight::ShaderLight(ID3D11Device* dev, WCHAR* filename)
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

	// light param
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_LightParams);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBufferLightParams);
	assert(SUCCEEDED(hr));

	// Color buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Data_WorldAndMaterial);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBuffWordAndMaterial);
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

	// Structured buffer for point lights
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(PointLight) * MAX_POINT_LIGHTS;
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = sizeof(PointLight);
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBufferPointLights);
	assert(SUCCEEDED(hr));

	// Structured buffer for spotlights
	bd.ByteWidth = sizeof(SpotLight) * MAX_SPOT_LIGHTS;
	bd.StructureByteStride = sizeof(SpotLight);
	hr = this->GetDevice()->CreateBuffer(&bd, nullptr, &mpBufferSpotLights);
	assert(SUCCEEDED(hr));

	// Create SRVs for the structured buffers
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = MAX_POINT_LIGHTS;

	hr = dev->CreateShaderResourceView(mpBufferPointLights, &srvDesc, &mpBufferPointLightsSRV);
	assert(SUCCEEDED(hr));

	srvDesc.Buffer.NumElements = MAX_SPOT_LIGHTS;

	hr = dev->CreateShaderResourceView(mpBufferSpotLights, &srvDesc, &mpBufferSpotLightsSRV);
	assert(SUCCEEDED(hr));

	// Zeroing the light data
	ZeroMemory(&DirLightData, sizeof(DirLightData));
	ZeroMemory(&PointLightDatas, sizeof(PointLightDatas));
	ZeroMemory(&SpotLightDatas, sizeof(SpotLightDatas));

	// Default initialization for essential settings
	mNumPointLights = 0;
	mNumSpotLights = 0;
	DirLightData.Direction = Vect(1.0f, 1.0f, 1.0f, 0.0f);
	DirLightData.Light.Ambient = Vect(1.0f, 1.0f, 1.0f, 0.0f);
	DirLightData.Light.Diffuse = Vect(1.0f, 1.0f, 1.0f, 0.0f);
	DirLightData.Light.Specular = Vect(1.0f, 1.0f, 1.0f, 0.0f);

	// Default initialization for essential settings
	FogPara.FogStart = FLT_MAX; // Disable fog
	FogPara.FogRange = 1.0f;    // Set to 1 (doesn't matter since FogStart is FLT_MAX)
	FogPara.FogColor = Vect(0.0f, 0.0f, 0.0f, 1.0f);
	FogPara.FogBaseHeight = 0.0f;
	FogPara.FogThickness = 0.0f;
	FogPara.ExpFogDensity = 1.0f;
	FogPara.Opacity = 1.0f;
	FogPara.FogType = (int)FogType::Undefined_Fog;
}

ShaderLight::~ShaderLight()
{
	ReleaseAndDeleteCOMobject(mpBuffWordAndMaterial);
	ReleaseAndDeleteCOMobject(mpBufferLightParams);
	ReleaseAndDeleteCOMobject(mpBufferCamMatrices);
	ReleaseAndDeleteCOMobject(mpBufferFog);
	ReleaseAndDeleteCOMobject(mpBufferPointLights);
	ReleaseAndDeleteCOMobject(mpBufferSpotLights);
	ReleaseAndDeleteCOMobject(mpBufferPointLightsSRV);
	ReleaseAndDeleteCOMobject(mpBufferSpotLightsSRV);
}

void ShaderLight::SendCamMatrices(const Matrix& view, const Matrix& proj)
{
	CamMatrices	mCamMatrices;
	mCamMatrices.View = view;
	mCamMatrices.Projection = proj;

	this->GetContext()->UpdateSubresource(mpBufferCamMatrices, 0, nullptr, &mCamMatrices, 0, 0);
}

void ShaderLight::SetFog(int fogType, const Vect& color, float start, float range,
	float thickness, float BaseHeight, float expFogDensity, float opacity)
{
	assert(start > 0.0f && range > 0.0f && thickness > 0.0f && expFogDensity > 0.0f);
	assert(fogType == 0 || fogType == 1 || fogType == 2 || fogType == 3);
	assert(opacity >= 0.0f && opacity <= 1.0f);

	FogPara.FogStart = start;
	FogPara.FogRange = range;
	FogPara.FogColor = color;
	FogPara.FogThickness = thickness;
	FogPara.FogBaseHeight = BaseHeight;
	FogPara.ExpFogDensity = expFogDensity;
	FogPara.Opacity = opacity;
	FogPara.FogType = fogType;
}

void ShaderLight::SetDirectionalLight(const Vect& dir, const Vect& amb, const Vect& dif, const Vect& sp)
{
	DirLightData.Light.Ambient = amb;
	DirLightData.Light.Diffuse = dif;
	DirLightData.Light.Specular = sp;
	DirLightData.Direction = dir;
}

void ShaderLight::AddPointLight(const Vect& pos, float r,
	const Vect& att, const Vect& amb, const Vect& dif, const Vect& sp)
{
	if (mNumPointLights >= MAX_POINT_LIGHTS)
	{
		// Log an error or throw an exception if needed
		OutputDebugStringW(L"Error: Exceeded maximum supported point lights.\n");
		assert(false); // Exit the function to avoid writing out of bounds
	}

	PointLightDatas[mNumPointLights].Light.Ambient = amb;
	PointLightDatas[mNumPointLights].Light.Diffuse = dif;
	PointLightDatas[mNumPointLights].Light.Specular = sp;
	PointLightDatas[mNumPointLights].Position = pos;
	PointLightDatas[mNumPointLights].Attenuation = att;
	PointLightDatas[mNumPointLights].Range = r;

	mNumPointLights++;
}

void ShaderLight::AddSpotLight(
	const Vect& pos, float r, const Vect& att, const Vect& dir, float spotExp,
	const Vect& amb, const Vect& dif, const Vect& sp)
{
	if (mNumPointLights >= MAX_SPOT_LIGHTS)
	{
		// Log an error or throw an exception if needed
		OutputDebugStringW(L"Error: Exceeded maximum supported spot lights.\n");
		assert(false); // Exit the function to avoid writing out of bounds
	}

	SpotLightDatas[mNumSpotLights].Light.Ambient = amb;
	SpotLightDatas[mNumSpotLights].Light.Diffuse = dif;
	SpotLightDatas[mNumSpotLights].Light.Specular = sp;
	SpotLightDatas[mNumSpotLights].Position = pos;
	SpotLightDatas[mNumSpotLights].Direction = dir;
	SpotLightDatas[mNumSpotLights].Attenuation = att;
	SpotLightDatas[mNumSpotLights].Range = r;
	SpotLightDatas[mNumSpotLights].SpotExp = spotExp;

	mNumSpotLights++;
}

void ShaderLight::SendLightParameters(const Vect& eyepos)
{
	Data_LightParams dl;
	dl.DirLight = DirLightData;
	dl.NumPointLights = mNumPointLights;
	dl.NumSpotLights = mNumSpotLights;
	dl.EyePosWorld = eyepos;

	this->GetContext()->UpdateSubresource(mpBufferLightParams, 0, nullptr, &dl, 0, 0);

	// Update point lights structured buffer
	if (mNumPointLights > 0)
	{
		this->GetContext()->UpdateSubresource(mpBufferPointLights, 0, nullptr, PointLightDatas, 0, 0);
	}

	// Update spotlights structured buffer
	if (mNumSpotLights > 0)
	{
		this->GetContext()->UpdateSubresource(mpBufferSpotLights, 0, nullptr, SpotLightDatas, 0, 0);
	}
}

void ShaderLight::SendWorldAndMaterial(const Matrix& world, const Vect& amb, const Vect& dif, const Vect& sp)
{
	Data_WorldAndMaterial wm;
	wm.World = world;
	wm.WorlInv = world.getInv();
	wm.Mat.Ambient = amb;
	wm.Mat.Diffuse = dif;
	wm.Mat.Specular = sp;

	this->GetContext()->UpdateSubresource(mpBuffWordAndMaterial, 0, nullptr, &wm, 0, 0);
}

void ShaderLight::SendFogParameters()
{
	this->GetContext()->UpdateSubresource(mpBufferFog, 0, nullptr, &FogPara, 0, 0);
}

void ShaderLight::SetToContext(ID3D11DeviceContext* devcon)
{
	ShaderBase::SaveContextPtr(devcon);
	ShaderBase::SetToContext_VS_PS_InputLayout();

	devcon->VSSetConstantBuffers(0, 1, &mpBufferCamMatrices);
	devcon->VSSetConstantBuffers(1, 1, &mpBufferLightParams);
	devcon->VSSetConstantBuffers(2, 1, &mpBuffWordAndMaterial);
	//devcon->PSSetConstantBuffers(3, 1, &mpBufferFog);

	devcon->PSSetConstantBuffers(0, 1, &mpBufferCamMatrices);
	devcon->PSSetConstantBuffers(1, 1, &mpBufferLightParams);
	devcon->PSSetConstantBuffers(2, 1, &mpBuffWordAndMaterial);
	devcon->PSSetConstantBuffers(3, 1, &mpBufferFog);

	// Bind SRVs for structured buffers
	ID3D11ShaderResourceView* srvs[] = {
		mpBufferPointLightsSRV, // SRV for point lights
		mpBufferSpotLightsSRV   // SRV for spotlights
	};
	devcon->PSSetShaderResources(1, 2, srvs); // Bind to t1 and t2
}

void ShaderLight::UpdateDirectionalLight(LightParam Param, const Vect& value)
{
	switch (Param)
	{
	case LightParam::DIR:
		DirLightData.Direction = value;
		break;
	case LightParam::AMB:
		DirLightData.Light.Ambient = value;
		break;

	case LightParam::DIFF:
		DirLightData.Light.Diffuse = value;
		break;

	case LightParam::SPEC:
		DirLightData.Light.Specular = value;
		break;

	default:
		OutputDebugStringW(L"Error: Invalid directional light parameter.\n");
		assert(false);
		break;
	}
}

void ShaderLight::UpdatePointLight(LightParam Param, int index, const Vect& value)
{
	if (index >= mNumPointLights)
	{
		OutputDebugStringW(L"Error: Attempted to modify a non-existent point light.\n");
		assert(false); // Exit the function to avoid writing out of bounds
	}

	switch (Param)
	{
	case LightParam::POS:
		PointLightDatas[index].Position = value;
		break;

	case LightParam::ATTE:
		PointLightDatas[index].Attenuation = value;
		break;

	case LightParam::AMB:
		PointLightDatas[index].Light.Ambient = value;
		break;

	case LightParam::DIFF:
		PointLightDatas[index].Light.Diffuse = value;
		break;

	case LightParam::SPEC:
		PointLightDatas[index].Light.Specular = value;
		break;

	default:
		OutputDebugStringW(L"Error: Invalid point light parameter.\n");
		assert(false); // Handle invalid parameter
		break;
	}
}

void ShaderLight::UpdatePointLight(LightParam Param, int index, float value)
{
	if (index >= mNumPointLights)
	{
		OutputDebugStringW(L"Error: Attempted to modify a non-existent point light.\n");
		assert(false); // Exit the function to avoid writing out of bounds
	}

	switch (Param)
	{
	case LightParam::RANGE:
	{
		PointLightDatas[index].Range = value;
		break;
	}

	default:
		OutputDebugStringW(L"Error: Invalid point light parameter.\n");
		assert(false); // Handle invalid parameter
		break;
	}
}

void ShaderLight::UpdateSpotLight(LightParam Param, int index, const Vect& value)
{
	if (index >= mNumSpotLights)
	{
		OutputDebugStringW(L"Error: Attempted to modify a non-existent spotlight.\n");
		assert(false); // Exit the function to avoid writing out of bounds
	}

	switch (Param)
	{
	case LightParam::POS:
		SpotLightDatas[index].Position = value;
		break;

	case LightParam::ATTE:
		SpotLightDatas[index].Attenuation = value;
		break;

	case LightParam::AMB:
		SpotLightDatas[index].Light.Ambient = value;
		break;

	case LightParam::DIFF:
		SpotLightDatas[index].Light.Diffuse = value;
		break;

	case LightParam::DIR:
		SpotLightDatas[index].Direction = value;
		break;

	case LightParam::SPEC:
		SpotLightDatas[index].Light.Specular = value;
		break;

	default:
		OutputDebugStringW(L"Error: Invalid spotlight parameter.\n");
		assert(false); // Handle invalid parameter
		break;
	}
}

void ShaderLight::UpdateSpotLight(LightParam Param, int index, float value)
{
	if (index >= mNumSpotLights)
	{
		OutputDebugStringW(L"Error: Attempted to modify a non-existent spotlight.\n");
		assert(false); // Exit the function to avoid writing out of bounds
	}

	switch (Param)
	{
	case LightParam::RANGE:
		SpotLightDatas[index].Range = value;
		break;

	case LightParam::SPOTEXP:
		SpotLightDatas[index].SpotExp = value;
		break;
	default:
		OutputDebugStringW(L"Error: Invalid spot light parameter.\n");
		assert(false); // Handle invalid parameter
		break;
	}
}

void ShaderLight::StartShaderPass(ID3D11DeviceContext* devcon,
	const Matrix& view, const Matrix& proj, const Vect& eyepos)
{
	this->SetToContext(devcon);
	this->SendCamMatrices(view, proj);
	this->SendLightParameters(eyepos);
	this->SendFogParameters();
}