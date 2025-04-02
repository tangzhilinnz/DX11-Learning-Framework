#include "Mirror.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <assert.h>

Mirror::Mirror(ID3D11Device* dev)
	: mDevice(dev),
	pMirrorObj(nullptr),
	MarkMirrorDSS(nullptr),
    DrawReflectionDSS(nullptr),
	DrawReflectionSkyboxDSS(nullptr),
    FrontFaceAsCCWRS(nullptr),
    MirrorFrontFaceAsClockWiseRS(nullptr),
    NoWriteToRenderTargetBS(nullptr),
    TransparentBS(nullptr)
{
	HRESULT hr = S_OK;

	// ***** Mirror Preliminaries ************

	D3D11_DEPTH_STENCIL_DESC mirrorDesc;
	ZeroMemory(&mirrorDesc, sizeof(mirrorDesc));
	mirrorDesc.DepthEnable = true;								// Use the depth test
	mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// but don't write to the depth buffer
	mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mirrorDesc.StencilEnable = true;							// Use the stencil test
	// Allows all 8 bits of the stencil buffer to participate in the stencil
	mirrorDesc.StencilReadMask = 0xff;
	// Allows writing all 8 bits to the stencil buffer.
	mirrorDesc.StencilWriteMask = 0xff;

	// If the stencil test fails, keep the existing stencil value.
	mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	// If the depth test fails, keep the existing stencil value.
	mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	// The stencil test always passes, meaning stencil marking will occur whenever geometry is drawn
	mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		// When passing the test (Always) replace
	// When the stencil test passes, replace the stencil value with the reference value set in the pipeline.
	mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;	// the color value with the reference value (int 1)

	// Back faces are culled, so these don't matter 
	mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = mDevice->CreateDepthStencilState(&mirrorDesc, &MarkMirrorDSS);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_DESC drawReflectionDesc;
	ZeroMemory(&drawReflectionDesc, sizeof(drawReflectionDesc));
	drawReflectionDesc.DepthEnable = true;								// Use the depth test
	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// normally
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawReflectionDesc.StencilEnable = true;							// Use the stencil test
	drawReflectionDesc.StencilReadMask = 0xff;
	drawReflectionDesc.StencilWriteMask = 0xff;

	// If the stencil test fails, keep the existing stencil value.
	drawReflectionDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	// This ensures that reflections are only drawn in pixels that were previously marked
	// with the reference value ¡ª ensuring reflections appear only inside the mirror area.
	drawReflectionDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;   // When passing the stencil comparison test,
	drawReflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	 // do not modify the stencil

	// Back faces are culled, so these don't matter
	drawReflectionDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	hr = mDevice->CreateDepthStencilState(&drawReflectionDesc, &DrawReflectionDSS);
	assert(SUCCEEDED(hr));

	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Pass if depth is less or equal
	hr = mDevice->CreateDepthStencilState(&drawReflectionDesc, &DrawReflectionSkyboxDSS);
	assert(SUCCEEDED(hr));

	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;   // D3D11_FILL_WIREFRAME
	rd.CullMode = D3D11_CULL_BACK;
	// In mirror rendering, the reflected geometry often needs to reverse its winding order
	// to maintain correct front/back face culling.
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;
	rd.AntialiasedLineEnable = false;

	hr = mDevice->CreateRasterizerState(&rd, &MirrorFrontFaceAsClockWiseRS);
	assert(SUCCEEDED(hr));

	rd.FillMode = D3D11_FILL_SOLID;  // Also: D3D11_FILL_WIREFRAME
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = true; // true for RH forward facing
	rd.DepthBias = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;  // Does not in fact turn on/off multisample: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
	rd.AntialiasedLineEnable = false;

	//ID3D11RasterizerState* rs;
	hr = mDevice->CreateRasterizerState(&rd, &FrontFaceAsCCWRS);
	assert(SUCCEEDED(hr));

	// These settings implement the standard alpha blending: c_src*(a_src) + c_dst*(1-a_src) 
	D3D11_BLEND_DESC bd;
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	// Enables writing to all color channels (Red, Green, Blue, Alpha).
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = mDevice->CreateBlendState(&bd, &TransparentBS);
	assert(SUCCEEDED(hr));

	// These setting prevent all writings to the render target 
	// (the key is that RenderTargetWriteMask = 0, so nothing is allowed)
	// see https://msdn.microsoft.com/en-us/library/windows/desktop/bb204901(v=vs.85).aspx
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = false;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	// This means that no color data will be written to the render target.
	// This is useful for operations such as depth-only or stencil-only
	// rendering, where you want to modify the depth or stencil buffer
	// but not the color buffer.
	bd.RenderTarget[0].RenderTargetWriteMask = 0;

	hr = mDevice->CreateBlendState(&bd, &NoWriteToRenderTargetBS);
	assert(SUCCEEDED(hr));

	// *********************************
	this->mReflectionMat = Matrix(IDENTITY);
}

void Mirror::StartReflection(ID3D11DeviceContext* devcon,
	const Matrix& viewMat, const Matrix& projMat, const Vect& eyepos, Skybox* skybox)
{
	assert(this->pMirrorObj && this->pMirrorObj->GetShader());
	// STEP 2: Marking the mirror pixels
	// BLEND STATE: Stop writing to the render target 
	devcon->OMSetBlendState(NoWriteToRenderTargetBS, nullptr, 0xffffffff);
	// STENCIL: Set up the stencil for marking ('1' for all pixels that passed the depth test. See comment at line 35)
	devcon->OMSetDepthStencilState(MarkMirrorDSS, 1);
	// Render the mirror 
	this->pMirrorObj->GetShader()->StartShaderPass(devcon, viewMat, projMat, eyepos);
	this->pMirrorObj->Render();
	// STENCIL: stop using the stencil
	devcon->OMSetDepthStencilState(0, 0);
	// BLEND STATE: Return the blend state to normal (writing to render target)
	devcon->OMSetBlendState(0, nullptr, 0xffffffff);

	// STEP 3:  Render objects (to render target) that should be reflected in the mirror
	// WINDINGS: face winding will appear inside out after reflection. Switching to CW front facing
	devcon->RSSetState(MirrorFrontFaceAsClockWiseRS);

	if (skybox)
	{
		devcon->OMSetDepthStencilState(DrawReflectionSkyboxDSS, 1);
		Vect eyeposRef = eyepos * this->mReflectionMat;
		Matrix skyboxViewMat = viewMat;
		skyboxViewMat.set(ROW_3, Vect(0.0f, 0.0f, 0.0f, 1.0f));
		skybox->GetShader()->StartShaderPass(devcon, skyboxViewMat, projMat, eyeposRef);
		skybox->Render();
		devcon->OMSetDepthStencilState(0, 0);
	}

	// STENCIL: Use the stencil test (reference value 1) and only pass the test if the stencil already had a one present
	devcon->OMSetDepthStencilState(DrawReflectionDSS, 1);
}

void Mirror::EndReflection(ID3D11DeviceContext* devcon, const Matrix& viewMat,
	const Matrix& projMat, const Vect& eyepos)
{
	assert(this->pMirrorObj && this->pMirrorObj->GetShader());

	// STENCIL: Stop using the stencil
	devcon->OMSetDepthStencilState(0, 0);
	// WINDING: back to normal windings
	devcon->RSSetState(FrontFaceAsCCWRS);

	// STEP 4: Render the mirror again, using alpha blending to 'see' the reflection 'through' it
	// BLENDING: Sets standard alpha blending: c_src*(a_src) + c_dst*(1-a_src)
	devcon->OMSetBlendState(TransparentBS, nullptr, 0xffffffff);
	this->pMirrorObj->GetShader()->StartShaderPass(devcon, viewMat, projMat, eyepos);
	pMirrorObj->Render();
	// BLENDING: Back to normal blending (turn off transparent blending)
	devcon->OMSetBlendState(0, nullptr, 0xffffffff);
}

void Mirror::SetMirrorMat(const Matrix& mirrorMat)
{
	assert(this->pMirrorObj);
	this->pMirrorObj->SetWorld(mirrorMat);
	this->priComputeReflectionMat(this->pMirrorObj->GetWorldMatrix());
}

void Mirror::SetMirrorObj(GraphicObject_Color* mirrorObj)
{
	assert(mirrorObj);
	this->pMirrorObj = mirrorObj;
	this->priComputeReflectionMat(this->pMirrorObj->GetWorldMatrix());
}

GraphicObject_Color* Mirror::GetMirrorObj()
{
	return pMirrorObj;
}

Matrix Mirror::GetMirrorMat() const
{
	return this->pMirrorObj->GetWorldMatrix();
}

const Matrix& Mirror::GetReflectionMat() const
{
	return this->mReflectionMat;
}

void Mirror::priComputeReflectionMat(const Matrix& mirrorMat)
{
	// Create Reflection matrix for mirror plane.
	// Uses the mirror's normal N and position
	Vect N = mirrorMat.get(ROW_1); // Mirror normal
	Vect p = mirrorMat.get(ROW_3); // point on mirror
	float d = -p.dot(N);
	Matrix NTN(N[x] * N, N[y] * N, N[z] * N, d * N);  // Set the 4 rows
	NTN[m3] = 0;
	NTN[m7] = 0;
	NTN[m11] = 0;
	this->mReflectionMat = Matrix(IDENTITY) - 2 * NTN;
	this->mReflectionMat[m15] = 1;
}

Mirror::~Mirror()
{
	ReleaseAndDeleteCOMobject(MarkMirrorDSS);
	ReleaseAndDeleteCOMobject(DrawReflectionDSS);
	ReleaseAndDeleteCOMobject(DrawReflectionSkyboxDSS);
	ReleaseAndDeleteCOMobject(MirrorFrontFaceAsClockWiseRS);
	ReleaseAndDeleteCOMobject(TransparentBS);
	ReleaseAndDeleteCOMobject(NoWriteToRenderTargetBS);
	ReleaseAndDeleteCOMobject(FrontFaceAsCCWRS);
}