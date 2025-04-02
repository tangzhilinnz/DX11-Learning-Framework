#include "DXApp.h"
#include <windows.h>
#include <sstream>
#include <assert.h>
#include "d3dUtil.h"

// needed to load shaders from file
#include <d3dcompiler.h>

#include "Model.h"
#include "ShaderColor.h"
#include "DirectXTex.h"
#include "Texture.h"
#include "FlatPlane.h"
#include "TerrainModel.h"
#include "SkyboxModel.h"
#include "GraphicObject_Color.h"
#include "GraphicObject_Texture.h"
#include "GraphicObject_ColorLight.h"
#include "GraphicObject_TextureLight.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Water.h"
#include "Mirror.h"

DXApp::DXApp(HWND hwnd)
{
	assert(hwnd);
	mhMainWnd = hwnd;

	BackgroundColor = Colors::MidnightBlue;

	md3dDevice = nullptr;
	md3dImmediateContext = nullptr;
	mSwapChain = nullptr;
	mRenderTargetView = nullptr;
	mSkyboxDesc = nullptr;
	mWaterBlendEffect = nullptr;

	pShaderCol = nullptr;
	pShaderTexLight = nullptr;
	pShaderTexLightRef = nullptr;

	pShaderLightHouse = nullptr;
	pShaderLightHouseRef = nullptr;

	pPlane = nullptr;
	pPlaneObj = nullptr;

	pCube = nullptr;

	pMirror = nullptr;
	pMirrorObj = nullptr;

	pSkyboxModel = nullptr;
	pSkyboxObj = nullptr;
	pSkyboxObjRef = nullptr;
	pSkyboxTex = nullptr;
	pShaderSkybox = nullptr;
	pShaderSkyboxRef = nullptr;

	pMirrorMan = nullptr;

	pShaderTerrain = nullptr;
	pTerrain = nullptr;
	pTerrainObj = nullptr;
	rockTex = nullptr;
	cliffTex = nullptr;
	rockTexNM = nullptr;
	cliffTexNM = nullptr;

	pSphere = nullptr;
	pSphereObj = nullptr;
	sphereTex = nullptr;
	sphereTexNM = nullptr;

	pCubeObj1 = nullptr;
	pCubeObj2 = nullptr;
	pPyramid = nullptr;
	pPyramidObj = nullptr;

	pLightHouse = nullptr;
	pLightHouseTex0 = nullptr;
	pLightHouseTex1 = nullptr;
	pLightHouseTex0NM = nullptr;
	pLightHouseTex1NM = nullptr;
	pLightHouseObj = nullptr;
	pLightHouseObjRef = nullptr;

	//pBoat = nullptr;
	//pBoatTex = nullptr;
	//pBoatTexNM = nullptr;
	//pBoatObj = nullptr;

	pShaderWater = nullptr;
	pWater = nullptr;
	pWaterObj = nullptr;
	waterHeightMap = nullptr;
	mHeigtMapSize = 0;
	mWaterXZSize = 0.f;
	mWaterYSize = 0.f;

	pLamp = nullptr;
	pLampTex = nullptr;
	pLampTexNM = nullptr;
	pLampObj1 = nullptr;
	pLampObj2 = nullptr;
	pLampObj1Ref = nullptr;
	pLampObj2Ref = nullptr;

	pShip = nullptr;
	pShipTex1 = nullptr;
	pShipTex1NM = nullptr;
    pShipObj = nullptr;

	pRock1 = nullptr;
	pRockTex1 = nullptr;
	pRockTex1NM = nullptr;
	pRockObj1 = nullptr;

	pRock2 = nullptr;
	pRockTex2 = nullptr;
	pRockTex2NM = nullptr;
	pRockObj2 = nullptr;

	pRock3 = nullptr;
	pRockTex3 = nullptr;
	pRockTex3NM = nullptr;
	pRockObj3 = nullptr;

	// Get window data through the window handle
	RECT rc;
	BOOL err = GetClientRect(mhMainWnd, &rc);  // Seriously MS: Redifining BOOL as int? Confusing much?
	assert(err);

	// get width/hight
	mClientWidth = rc.right - rc.left;
	mClientHeight = rc.bottom - rc.top;

	// Get window caption
	const int MAX_LABEL_LENGTH = 100; // probably overkill...
	CHAR str[MAX_LABEL_LENGTH];
	GetWindowText(mhMainWnd, str, MAX_LABEL_LENGTH);
	mMainWndCaption = str;

	// Initialize DX11
	this->InitDirect3D();

	// Demo initialization
	this->InitDemo();
}

void DXApp::InitDirect3D()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This is a *greatly* simplified process to create a DX device and context:
	// We force the use of DX11 feature level since that's what CDM labs are limited to.
	// For real-life applications would need to test what's the best feature level and act accordingly
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &md3dDevice, nullptr, &md3dImmediateContext);
	assert(SUCCEEDED(hr));

	// Now we obtain the associated DXGIfactory1 with our device 
	// Many steps...
	IDXGIDevice* dxgiDevice = nullptr;
	hr = md3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	assert(SUCCEEDED(hr));

	IDXGIAdapter* adapter = nullptr;
	hr = dxgiDevice->GetAdapter(&adapter);
	assert(SUCCEEDED(hr));

	IDXGIFactory1* dxgiFactory1 = nullptr;
	hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory1));
	assert(SUCCEEDED(hr));
	// See also note on weird stuff with factories and swap chains (1s and 2s)
	// https://msdn.microsoft.com/en-us/library/windows/desktop/jj863687(v=vs.85).aspx

	// We are done with these now...
	ReleaseAndDeleteCOMobject(adapter);
	ReleaseAndDeleteCOMobject(dxgiDevice);

	// Controls MSAA option:
	// - 4x count level garanteed for all DX11 
	// - MUST be the same for depth buffer!
	// - We _need_ to work with the depth buffer because reasons... (see below)
	DXGI_SAMPLE_DESC sampDesc;
	sampDesc.Count = 1;
	sampDesc.Quality = static_cast<UINT>(D3D11_CENTER_MULTISAMPLE_PATTERN);  // MS: what's with the type mismtach?

	DXGI_MODE_DESC buffdesc;				// https://msdn.microsoft.com/en-us/library/windows/desktop/bb173064(v=vs.85).aspx
	ZeroMemory(&buffdesc, sizeof(buffdesc));
	buffdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Next we create a swap chain. 
	// Useful thread: http://stackoverflow.com/questions/27270504/directx-creating-the-swapchain
	// Note that this is for a DirectX 11.0: in a real app, we should test the feature levels and act accordingly

	DXGI_SWAP_CHAIN_DESC sd;				// See MSDN: https://msdn.microsoft.com/en-us/library/windows/desktop/bb173075(v=vs.85).aspx
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;						// Much confusion about this number... see http://www.gamedev.net/topic/633807-swap-chain-buffer-count/
	sd.BufferDesc = buffdesc;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mhMainWnd;
	sd.SampleDesc = sampDesc;
	sd.Windowed = TRUE;

	hr = dxgiFactory1->CreateSwapChain(md3dDevice, &sd, &mSwapChain);
	assert(SUCCEEDED(hr));
	ReleaseAndDeleteCOMobject(dxgiFactory1);

	// Create a render target view		https://msdn.microsoft.com/en-us/library/windows/desktop/ff476582(v=vs.85).aspx
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	assert(SUCCEEDED(hr));;

	hr = md3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
	ReleaseAndDeleteCOMobject(pBackBuffer);
	assert(SUCCEEDED(hr));

	/**********************************************************/

	// First we fix what it means for triangles to be front facing.
	// Requires setting a whole new rasterizer state
	D3D11_RASTERIZER_DESC rd;
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

	ID3D11RasterizerState* rs;
	md3dDevice->CreateRasterizerState(&rd, &rs);

	md3dImmediateContext->RSSetState(rs);
	ReleaseAndDeleteCOMobject(rs); // we can release this resource since we won't be changing it any further

	// We must turn on the abilty to process depth during rendering.
	// Done through depth stencils (see https://msdn.microsoft.com/en-us/library/windows/desktop/bb205074(v=vs.85).aspx)
	// Below is a simplified version
	//*
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = mClientWidth;
	descDepth.Height = mClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc = sampDesc;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencil;
	hr = md3dDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	assert(SUCCEEDED(hr));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;;

	hr = md3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &mpDepthStencilView);
	assert(SUCCEEDED(hr));
	ReleaseAndDeleteCOMobject(pDepthStencil);

	// For drawing skybox
	D3D11_DEPTH_STENCIL_DESC skyboxDepthDesc;
	skyboxDepthDesc.DepthEnable = TRUE;               // Enable depth testing
	skyboxDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable depth writes
	skyboxDepthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Pass if depth is less or equal
	skyboxDepthDesc.StencilEnable = FALSE;            // No stencil needed
	hr = md3dDevice->CreateDepthStencilState(&skyboxDepthDesc, &mSkyboxDesc);
	assert(SUCCEEDED(hr));

	ReleaseAndDeleteCOMobject(pDepthStencil);

	// For blending water and terrain
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
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = md3dDevice->CreateBlendState(&bd, &mWaterBlendEffect);
	assert(SUCCEEDED(hr));



	D3D11_DEPTH_STENCIL_DESC shipDesc;
	ZeroMemory(&shipDesc, sizeof(shipDesc));
	shipDesc.DepthEnable = true;								// Use the depth test
	shipDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// but don't write to the depth buffer
	shipDesc.DepthFunc = D3D11_COMPARISON_LESS;
	shipDesc.StencilEnable = true;							// Use the stencil test
	// Allows all 8 bits of the stencil buffer to participate in the stencil
	shipDesc.StencilReadMask = 0xff;
	// Allows writing all 8 bits to the stencil buffer.
	shipDesc.StencilWriteMask = 0xff;

	// If the stencil test fails, keep the existing stencil value.
	shipDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	// If the depth test fails, keep the existing stencil value.
	shipDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	// The stencil test always passes, meaning stencil marking will occur whenever geometry is drawn
	shipDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		// When passing the test (Always) replace
	// When the stencil test passes, replace the stencil value with the reference value set in the pipeline.
	shipDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;	// the color value with the reference value (int 1)

	// Back faces are culled, so these don't matter 
	shipDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	shipDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	shipDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	shipDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = md3dDevice->CreateDepthStencilState(&shipDesc, &MarkShipDSS);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_DESC drawWaterDesc;
	ZeroMemory(&drawWaterDesc, sizeof(drawWaterDesc));
	drawWaterDesc.DepthEnable = true;								// Use the depth test
	drawWaterDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// normally
	drawWaterDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawWaterDesc.StencilEnable = true;							// Use the stencil test
	drawWaterDesc.StencilReadMask = 0xff;
	drawWaterDesc.StencilWriteMask = 0xff;

	// If the stencil test fails, keep the existing stencil value.
	drawWaterDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawWaterDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	// This ensures that reflections are only drawn in pixels that were previously marked
	// with the reference value ¡ª ensuring reflections appear only inside the mirror area.
	drawWaterDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	drawWaterDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// Back faces are culled, so these don't matter
	drawWaterDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawWaterDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawWaterDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawWaterDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	hr = md3dDevice->CreateDepthStencilState(&drawWaterDesc, &DrawWaterDSS);
	assert(SUCCEEDED(hr));

	/**********************************************************/

	//md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);  // to use without depth stencil
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mpDepthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)mClientWidth;
	vp.Height = (FLOAT)mClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	md3dImmediateContext->RSSetViewports(1, &vp);
}

void DXApp::InitDemo()
{
	TEX::Init(20);

	mWaterXZSize = 2048.f;
	mWaterYSize = 8.f;

	pMirrorMan = new Mirror(md3dDevice);
	pShaderCol = new ShaderColor(md3dDevice);
	pShaderTexLight = new ShaderTextureLight(md3dDevice);
	pShaderTexLightRef = new ShaderTextureLight(md3dDevice);
	pShaderSkybox = new ShaderTexture(md3dDevice);
	pShaderSkyboxRef = new ShaderTexture(md3dDevice);
	pShaderTerrain = new ShaderTerrain(md3dDevice);
	pShaderWater = new ShaderWater(md3dDevice);
	pShaderLightHouse = new ShaderTextureLight(md3dDevice, L"../Assets/Shaders/LightHouse.hlsl");
	pShaderLightHouseRef = new ShaderTextureLight(md3dDevice, L"../Assets/Shaders/LightHouse.hlsl");

	pMirror = new FlatPlane(md3dDevice, 45.f, 1.f, 1.f);
	pMirrorObj = new GraphicObject_Color(pShaderCol, pMirror->GetModel());

	mirrorMat = Matrix(ROT_Y, 3.1415f / 2) * Matrix(TRANS, 22.f, -75.2f + 100.f, 32.f);
	pMirrorObj->SetWorld(mirrorMat);
	pMirrorObj->SetColor(Vect(0.39f, 0.58f, 0.93f, .43f));

	pMirrorMan->SetMirrorObj(pMirrorObj);

	pSkyboxTex = TEX::CreateTexObj(md3dDevice);
	pSkyboxTex->LoadTexture(L"../Assets/Textures/Skybox/skybox2.tga");
	pSkyboxTex->LoadSampler(D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT, 0,
		D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_MIRROR
	);

	cliffTex = TEX::CreateTexObj(md3dDevice);
	cliffTex->LoadTexture(L"../Assets/Textures/Terrain/cliffFace.tga", true, 0, DirectX::TEX_FILTER_LINEAR);
	cliffTex->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	rockTex = TEX::CreateTexObj(md3dDevice);
	rockTex->LoadTexture(L"../Assets/Textures/Terrain/rockFace.tga", true, 0, DirectX::TEX_FILTER_LINEAR);
	rockTex->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	rockTexNM = TEX::CreateTexObj(md3dDevice);
	rockTexNM->LoadTexture(L"../Assets/Textures/Terrain/rockFaceNM.tga", true, 0);
	rockTexNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	cliffTexNM = TEX::CreateTexObj(md3dDevice);
	cliffTexNM->LoadTexture(L"../Assets/Textures/Terrain/cliffFaceNM.tga", true, 0);
	cliffTexNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	sphereTex = TEX::CreateTexObj(md3dDevice);
	sphereTexNM = TEX::CreateTexObj(md3dDevice);

	sphereTex->LoadTexture(L"../Assets/Textures/RockWall.tga");
	sphereTex->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);
	sphereTexNM->LoadTexture(L"../Assets/Textures/RockWallNM.tga");
	sphereTexNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	pLightHouseTex0 = TEX::CreateTexObj(md3dDevice);
	pLightHouseTex0->LoadTexture(L"../Assets/Textures/LightHouse/TopColor.tga", true, 0);
	pLightHouseTex0->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	pLightHouseTex1 = TEX::CreateTexObj(md3dDevice);
	pLightHouseTex1->LoadTexture(L"../Assets/Textures/LightHouse/BaseColor.tga", true, 0);
	pLightHouseTex1->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	pLightHouseTex0NM = TEX::CreateTexObj(md3dDevice);
	pLightHouseTex0NM->LoadTexture(L"../Assets/Textures/LightHouse/TopNormal.tga", true, 0);
	pLightHouseTex0NM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	pLightHouseTex1NM = TEX::CreateTexObj(md3dDevice);
	pLightHouseTex1NM->LoadTexture(L"../Assets/Textures/LightHouse/BaseNormal.tga", true, 0);
	pLightHouseTex1NM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	waterHeightMap = TEX::CreateTexObj(md3dDevice);
	waterHeightMap->LoadTexture(L"../Assets/Textures/Water/WaveHeightMap.tga", true, 0);
	waterHeightMap->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, 0);

	//pBoatTex = TEX::CreateTexObj(md3dDevice);
	//pBoatTex->LoadTexture(L"../Assets/Textures/Boat/Boat.tga", true, 0);
	//pBoatTex->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	//pBoatTexNM = TEX::CreateTexObj(md3dDevice);
	//pBoatTexNM->LoadTexture(L"../Assets/Textures/Boat/BoatNM.tga", true, 0);
	//pBoatTexNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	pLampTex = TEX::CreateTexObj(md3dDevice);
	pLampTex->LoadTexture(L"../Assets/Textures/Lamp/Lamp.tga", true, 0);
	pLampTex->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	pLampTexNM = TEX::CreateTexObj(md3dDevice);
	pLampTexNM->LoadTexture(L"../Assets/Textures/Lamp/LampNM.tga", true, 0);
	pLampTexNM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	pShipTex1 = TEX::CreateTexObj(md3dDevice);
	pShipTex1->LoadTexture(L"../Assets/Textures/Ship2/Ship1.tga", true, 0);
	pShipTex1->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	pShipTex1NM = TEX::CreateTexObj(md3dDevice);
	pShipTex1NM->LoadTexture(L"../Assets/Textures/Ship2/Ship1NM.tga", true, 0);
	pShipTex1NM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, 0);

	pRockTex1 = TEX::CreateTexObj(md3dDevice);
	pRockTex1->LoadTexture(L"../Assets/Textures/Rock/Rock1.tga", true, 0);
	pRockTex1->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	pRockTex1NM = TEX::CreateTexObj(md3dDevice);
	pRockTex1NM->LoadTexture(L"../Assets/Textures/Rock/Rock1NM.tga", true, 0);
	pRockTex1NM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT);

	pRockTex2 = TEX::CreateTexObj(md3dDevice);
	pRockTex2->LoadTexture(L"../Assets/Textures/Rock/Rock2.tga", true, 0);
	pRockTex2->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	pRockTex2NM = TEX::CreateTexObj(md3dDevice);
	pRockTex2NM->LoadTexture(L"../Assets/Textures/Rock/Rock2NM.tga", true, 0);
	pRockTex2NM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT);

	pRockTex3 = TEX::CreateTexObj(md3dDevice);
	pRockTex3->LoadTexture(L"../Assets/Textures/Rock/Rock3.tga", true, 0);
	pRockTex3->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	pRockTex3NM = TEX::CreateTexObj(md3dDevice);
	pRockTex3NM->LoadTexture(L"../Assets/Textures/Rock/Rock3NM.tga", true, 0);
	pRockTex3NM->LoadSampler(D3D11_FILTER_MIN_MAG_MIP_POINT);

	const Matrix& ReflectionMat = pMirrorMan->GetReflectionMat();

	pPlane = new FlatPlane(md3dDevice, 30.f, 4.f, 4.f);
	planeMat = Matrix(SCALE, 0.8f, 0.8f, 0.8f) * Matrix(TRANS, 0.f, -72.35f + 100.f, -16.f);

	pCube = new Model(md3dDevice, Model::PreMadeModels::UnitBoxRepeatedTexture);
	spotLightPos = Vect(0.f, 46.f, -22.8f);

	mSkyboxMat = Matrix(IDENTITY);

	pSkyboxModel = new SkyboxModel(md3dDevice, 1000.f);
	pSphere = new Model(md3dDevice, Model::PreMadeModels::UnitSphere);
	pPyramid = new Model(md3dDevice, Model::PreMadeModels::UnitPyramidRepeatedTexture);

	pLightHouse = new Model(md3dDevice, "../Assets/Models/LightHouse.azul");
	mWorldLightHouse = Matrix(SCALE, Vect(6.f, 6.f, 6.f)) * Matrix(ROT_X, -3.1415f / 2) * Matrix(TRANS, 0.f, -72.5f + +100.f, -16.8f);

	//pBoat = new Model(md3dDevice, "../Assets/Models/Boat.azul");
	//mWorldBoat = Matrix(SCALE, Vect(0.04f, 0.04f, 0.04f)) * Matrix(ROT_X, -3.1415f / 10) * Matrix(TRANS, -12.f, 25.f, -58.f);

	mCubeMat1 = Matrix(SCALE, Vect(4.f, 4.f, 4.f)) * Matrix(TRANS, -2.5f, -70.4f + 100.f, -28.f);
	mCubeMat2 = Matrix(SCALE, Vect(4.f, 4.f, 4.f)) * Matrix(TRANS, -7.5f, -70.4f + 100.f, -28.f);
	mPyramidMat = Matrix(SCALE, Vect(4.f, 4.f, 4.f)) *  Matrix(TRANS, -2.5f, -70.4f + 4.f + 100.f, -28.f);
	mSphereMat = Matrix(SCALE, Vect(2.f, 2.f, 2.f)) *  Matrix(TRANS, -7.5f, -70.4f + 4.f + 100.f, -28.f);

	pTerrain = new TerrainModel(md3dDevice, L"../Assets/Textures/Terrain/heightMap.tga", 300.f, 40.0f, -10.f, 11, 11);
	mTerrainMat = Matrix(TRANS, 0.0f, -100.0f + 100.f, 0.0f);
	pTerrainObj = new Terrain(pShaderTerrain, pTerrain->GetModel());
	pTerrainObj->SetMaterial(Vect(0.35f, 0.35f, 0.35f, 0.0f), Vect(0.9f, 0.9f, 0.9f, 0.0f), Vect(0.5f, 0.5f, 0.5f, 2.f));
	pTerrainObj->SetWorld(mTerrainMat);
	pTerrainObj->SetTexture(rockTex, 0);
	pTerrainObj->SetTexture(cliffTex, 1);
	pTerrainObj->SetNormalMap(rockTexNM, 0);
	pTerrainObj->SetNormalMap(cliffTexNM, 1);

	pWater = new TerrainModel(md3dDevice, L"../Assets/Textures/Water/WaveHeightMap.tga",
		mWaterXZSize, mWaterYSize, -20.f, 5, 5, &mHeigtMapSize);

	mWaterMat = Matrix(TRANS, 0.0f, -98.0f + 100.f, 0.0f);
	pWaterObj = new Water(pShaderWater, pWater->GetModel());
	pWaterObj->SetMaterial(Vect(1.f, 1.f, 1.f, 0.0f), Vect(1.f, 1.f, 1.f, 0.0f), Vect(1.6f, 1.6f, 1.6f, 32.f));
	pWaterObj->SetWorld(mWaterMat);
	pWaterObj->SetHeighMapTexture(waterHeightMap);

	pPlaneObj = new GraphicObject_TextureLight(pShaderTexLight, pPlane->GetModel());
	pSkyboxObj = new Skybox(pShaderSkybox, pSkyboxModel->GetModel());
	pSphereObj = new GraphicObject_TextureLight(pShaderTexLight, pSphere);
	pCubeObj1 = new GraphicObject_TextureLight(pShaderTexLight, pCube);
	pCubeObj2 = new GraphicObject_TextureLight(pShaderTexLight, pCube);
	pPyramidObj = new GraphicObject_TextureLight(pShaderTexLight, pPyramid);
	//pLightHouseObj = new GraphicObject_TextureLight(pShaderTexLight, pLightHouse
	pLightHouseObj = new GraphicObject_TextureLight(pShaderLightHouse, pLightHouse);
	//pBoatObj = new GraphicObject_TextureLight(pShaderTexLight, pBoat);

	pPlaneObj->SetWorld(planeMat);
	pSkyboxObj->SetWorld(mSkyboxMat);
	pSphereObj->SetWorld(mSphereMat);

	pCubeObj1->SetWorld(mCubeMat1);
	pCubeObj2->SetWorld(mCubeMat2);
	pPyramidObj->SetWorld(mPyramidMat);

	pPlaneObj->SetMaterial(Vect(1.f, 1.f, 1.f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(0.4f, 0.4f, 0.4f, 4.f));
	pPlaneObj->SetTexture(sphereTex);
	pPlaneObj->SetNormalMap(sphereTexNM);

	pSkyboxObj->SetTexture(pSkyboxTex);

	pLightHouseObj->SetMaterial(Vect(0.66f, 0.66f, 0.66f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(0.65f, 0.65f, 0.65f, 4.f));
	pLightHouseObj->SetTexture(pLightHouseTex0, 0);
	pLightHouseObj->SetTexture(pLightHouseTex1, 1);
	pLightHouseObj->SetNormalMap(pLightHouseTex0NM, 0);
	pLightHouseObj->SetNormalMap(pLightHouseTex1NM, 1);
	pLightHouseObj->SetWorld(mWorldLightHouse);

	//pBoatObj->SetMaterial(Vect(0.66f, 0.66f, 0.66f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(0.65f, 0.65f, 0.65f, 4.f));
	//pBoatObj->SetWorld(mWorldBoat);
	//pBoatObj->SetTexture(pBoatTex, 0);
	//pBoatObj->SetNormalMap(pBoatTexNM, 0);

	pLamp = new Model(md3dDevice, "../Assets/Models/Lamp.azul");
	pLampObj1 = new GraphicObject_TextureLight(pShaderTexLight, pLamp);
	pLampObj2 = new GraphicObject_TextureLight(pShaderTexLight, pLamp);
	mWorldLamp1 = Matrix(SCALE, .0005f, .0005f, .0005f) * Matrix(ROT_X, -3.1415f / 2) * Matrix(TRANS, -6.2f, 34.3f, -10.2f);
	mWorldLamp2 = Matrix(SCALE, .0005f, .0005f, .0005f) * Matrix(ROT_X, -3.1415f / 2) * Matrix(TRANS, 6.2f, 34.3f, -10.2f);

	pLampObj1->SetMaterial(Vect(1.f, 1.f, 1.f, 0.f), Vect(2.f, 2.f, 2.f, 0.f), Vect(1.25f, 1.25, 1.25, 64.f));
	pLampObj1->SetTexture(pLampTex);
	pLampObj1->SetNormalMap(pLampTexNM);
	pLampObj1->SetWorld(mWorldLamp1);
	pLampObj2->SetMaterial(Vect(1.f, 1.f, 1.f, 0.f), Vect(2.f, 2.f, 2.f, 0.f), Vect(1.25f, 1.25, 1.25, 64.f));
	pLampObj2->SetTexture(pLampTex);
	pLampObj2->SetNormalMap(pLampTexNM);
	pLampObj2->SetWorld(mWorldLamp2);

	mWorldShip = Matrix(SCALE, 2.f, 2.f, 2.f) * Matrix(ROT_X, -3.1415f / 2.2f) * Matrix(ROT_Y,  -3.1415f) * Matrix(TRANS, 85.f, 6.3f, -50.f);
	pShip = new Model(md3dDevice, "../Assets/Models/Ship2.azul");
	pShipObj = new GraphicObject_TextureLight(pShaderTexLight, pShip);
	pShipObj->SetMaterial(Vect(0.3f, 0.3f, 0.3f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(0.2f, 0.2f, 0.2f, 2.f));
	pShipObj->SetTexture(pShipTex1);
	pShipObj->SetNormalMap(pShipTex1NM);
	pShipObj->SetWorld(mWorldShip);

	mWorldRock1 = Matrix(SCALE, 4.025f, 4.025f, 4.025f) * Matrix(ROT_Y, 3.1415f / 3) * Matrix(TRANS, 85.f, 9.5f, -25.f);
	pRock1 = new Model(md3dDevice, "../Assets/Models/Rock1.azul");
	pRockObj1 = new GraphicObject_TextureLight(pShaderTexLight, pRock1);
	pRockObj1->SetMaterial(Vect(0.2f, 0.2f, 0.2f, 0.f), Vect(0.8f, 0.8f, 0.8f, 0.f), Vect(0.35f, 0.35f, 0.35f, 2.f));
	pRockObj1->SetTexture(pRockTex1);
	pRockObj1->SetNormalMap(pRockTex1NM);
	pRockObj1->SetWorld(mWorldRock1);

	mWorldRock2 = Matrix(SCALE, 5.025f, 5.025f, 5.025f) * Matrix(ROT_Y, 3.1415f / 6.2) * Matrix(TRANS, 82.f, 9.8f, -16.f);
	pRock2 = new Model(md3dDevice, "../Assets/Models/Rock2.azul");
	pRockObj2 = new GraphicObject_TextureLight(pShaderTexLight, pRock1);
	pRockObj2->SetMaterial(Vect(0.2f, 0.2f, 0.2f, 0.f), Vect(0.8f, 0.8f, 0.8f, 0.f), Vect(0.35f, 0.35f, 0.35f, 2.f));
	pRockObj2->SetTexture(pRockTex2);
	pRockObj2->SetNormalMap(pRockTex2NM);
	pRockObj2->SetWorld(mWorldRock2);

	mWorldRock3 = Matrix(SCALE, 6.825f, 6.825f, 6.825f) * Matrix(ROT_Y, 3.1415f / 4.2) * Matrix(TRANS, 70.f, 9.8f, -26.f);
	pRock3 = new Model(md3dDevice, "../Assets/Models/Rock2.azul");
	pRockObj3 = new GraphicObject_TextureLight(pShaderTexLight, pRock3);
	pRockObj3->SetMaterial(Vect(0.2f, 0.2f, 0.2f, 0.f), Vect(0.8f, 0.8f, 0.8f, 0.f), Vect(0.35f, 0.35f, 0.35f, 2.f));
	pRockObj3->SetTexture(pRockTex3);
	pRockObj3->SetNormalMap(pRockTex3NM);
	pRockObj3->SetWorld(mWorldRock3);


	// Reflected objects
	mWorldLightHouseRef = mWorldLightHouse * ReflectionMat;

	Matrix skyboxReflectionMat = ReflectionMat;
	skyboxReflectionMat.set(ROW_3, Vect(0.0f, 0.0f, 0.0f, 1.0f));
	mSkyboxMatRef = mSkyboxMat * skyboxReflectionMat;

	pSkyboxObjRef = new Skybox(pShaderSkyboxRef, pSkyboxModel->GetModel());
	//pLightHouseObjRef = new GraphicObject_TextureLight(pShaderTexLightRef, pLightHouse);
	pLightHouseObjRef = new GraphicObject_TextureLight(pShaderLightHouseRef, pLightHouse);

	pSkyboxObjRef->SetWorld(mSkyboxMatRef);

	pSkyboxObjRef->SetTexture(pSkyboxTex);

	pSphereObj->SetMaterial(Colors::White, Colors::White, Vect(0.8f, 0.8f, 0.8f, 132.f), 0);
	pCubeObj1->SetMaterial(Colors::White, Colors::White, Vect(0.8f, 0.8f, 0.8f, 132.f), 0);
	pCubeObj2->SetMaterial(Colors::White, Colors::White, Vect(0.8f, 0.8f, 0.8f, 132.f), 0);
	pPyramidObj->SetMaterial(Colors::White, Colors::White, Vect(0.8f, 0.8f, 0.8f, 132.f), 0);
	pSphereObj->SetTexture(sphereTex);
	pSphereObj->SetNormalMap(sphereTexNM);
	pCubeObj1->SetTexture(rockTex);
	pCubeObj1->SetNormalMap(rockTexNM);
	pCubeObj2->SetTexture(rockTex);
	pCubeObj2->SetNormalMap(rockTexNM);
	pPyramidObj->SetTexture(cliffTex);
	pPyramidObj->SetNormalMap(cliffTexNM);

	pLightHouseObjRef->SetMaterial(Vect(0.66f, 0.66f, 0.66f, 0.f), Vect(1.f, 1.f, 1.f, 0.f), Vect(0.65f, 0.65f, 0.65f, 4.f));
	pLightHouseObjRef->SetTexture(pLightHouseTex0, 0);
	pLightHouseObjRef->SetTexture(pLightHouseTex1, 1);
	pLightHouseObjRef->SetNormalMap(pLightHouseTex0NM, 0);
	pLightHouseObjRef->SetNormalMap(pLightHouseTex1NM, 1);
	pLightHouseObjRef->SetWorld(mWorldLightHouseRef);

	pLampObj1Ref = new GraphicObject_TextureLight(pShaderTexLightRef, pLamp);
	pLampObj2Ref = new GraphicObject_TextureLight(pShaderTexLightRef, pLamp);
	mWorldLamp1Ref = mWorldLamp1 * ReflectionMat;
	mWorldLamp2Ref = mWorldLamp2 * ReflectionMat;

	pLampObj1Ref->SetMaterial(Vect(1.f, 1.f, 1.f, 0.f), Vect(2.f, 2.f, 2.f, 0.f), Vect(1.25f, 1.25, 1.25, 64.f));
	pLampObj1Ref->SetTexture(pLampTex);
	pLampObj1Ref->SetNormalMap(pLampTexNM);
	pLampObj1Ref->SetWorld(mWorldLamp1Ref);
	pLampObj2Ref->SetMaterial(Vect(1.f, 1.f, 1.f, 0.f), Vect(2.f, 2.f, 2.f, 0.f), Vect(1.25f, 1.25, 1.25, 64.f));
	pLampObj2Ref->SetTexture(pLampTex);
	pLampObj2Ref->SetNormalMap(pLampTexNM);
	pLampObj2Ref->SetWorld(mWorldLamp2Ref);

	sunLightADS = { .05f * Vect(1.f, 1.f, 1.f), 0.25f * Vect(1.f, 1.f, 1.f), Vect(0.88f, 0.88f, 0.88f) };
	sunLightDir = Vect(-4.f, -4.f, 1.f, 0.0f);
	sunLightRefDir = sunLightDir * ReflectionMat;

	lampLightADS = { 0.06f * Vect(1.f, 1.f, 1.f), 0.8f * Vect(1.f, 1.f, 1.f), 0.68f * Vect(1.f, 1.f, 1.f) };
	lamp1LightPosition = Vect(0.0f, 0.0f, 0.0f, 1.0f) * mWorldLamp1 + Vect(0.0f, -1.0f, 0.0f, 1.0f);;
	lamp2LightPosition = Vect(0.0f, 0.0f, 0.0f, 1.0f) * mWorldLamp2 + Vect(0.0f, -1.0f, 0.0f, 1.0f);
	lampLightAttenuation = 0.58f * Vect(1.f, 0.22f, 0.2f);
	lampLightRange = 20.4f;
	lamp1LightPositionRef = lamp1LightPosition * ReflectionMat;
	lamp2LightPositionRef = lamp2LightPosition * ReflectionMat;

	spotLightADS = { .2f * Vect(1.f, 1.f, 1.f), 1.f * Vect(1.f, 1.f, 1.f), 1.f * Vect(0.6f, 0.6f, 0.6f) };
	spotLightPos = Vect(0.0f, 0.0f, 0.0f, 1.0f) * Matrix(TRANS, 0.f, 34.f, -20.f);
	spotLightAttenuation = .2f * Vect(1.f, 0.007f, 0.0f);
	spotLightDir = Vect(1.0f, 0.0f, -1.0f, 0.0f);
	spotLightExp = 1.0f;
	spotLightRange = 1600.0f;

	Vect directionalLightRefDir = sunLightDir * ReflectionMat;
	directionalLightRefDir.W() = 0.0f;

	Vect spotlightRefPos = spotLightPos * ReflectionMat;
	Vect spotlightRefDir = spotLightDir * ReflectionMat;
	spotlightRefDir.W() = 0.0f;

	//Ground Fog
	pShaderSkyboxRef->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 15.f, 100.f, 280.f, -260.f, 0.78f, 0.99f);
	pShaderSkybox->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 15.f, 100.f, 280.f, -120.f+ 100.f, 0.78f, 0.99f);
	pShaderTexLight->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 15.f, 100.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);
	pShaderTexLightRef->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 15.f, 100.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);
	pShaderTerrain->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.85f), 30.f, 100.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);
	pShaderWater->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 30.f, 100.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);
	pShaderCol->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 0.1f, 20.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);

	pShaderLightHouse->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 15.f, 100.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);
	pShaderLightHouseRef->SetFog((int)FogType::Layered_Fog, Vect(0.7f, 0.75f, 0.8f), 15.f, 100.f, 280.f, -120.f + 100.f, 0.78f, 0.99f);

	pShaderWater->SetDirectionalLight(
		sunLightDir.getNorm(),
		sunLightADS.Ambient,
		sunLightADS.Diffuse,
		sunLightADS.Specular);

	pShaderTerrain->SetDirectionalLight(
		sunLightDir.getNorm(),
		sunLightADS.Ambient,
		sunLightADS.Diffuse,
		sunLightADS.Specular);

	pShaderTerrain->AddPointLight(
		lamp1LightPosition,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderTerrain->AddPointLight(
		lamp2LightPosition,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderTexLight->SetDirectionalLight(
		sunLightDir.getNorm(),
		sunLightADS.Ambient,
		sunLightADS.Diffuse,
		sunLightADS.Specular);

	pShaderTexLight->AddPointLight(
		lamp1LightPosition,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderTexLight->AddPointLight(
		lamp2LightPosition,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderLightHouse->SetDirectionalLight(
		sunLightDir.getNorm(),
		sunLightADS.Ambient,
		sunLightADS.Diffuse,
		sunLightADS.Specular);

	pShaderLightHouse->AddPointLight(
		lamp1LightPosition,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderLightHouse->AddPointLight(
		lamp2LightPosition,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderTexLightRef->SetDirectionalLight(
		directionalLightRefDir.getNorm(),
		sunLightADS.Ambient,
		sunLightADS.Diffuse,
		sunLightADS.Specular);

	pShaderTexLightRef->AddPointLight(
		lamp1LightPositionRef,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderTexLightRef->AddPointLight(
		lamp2LightPositionRef,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderTexLight->AddSpotLight(
		spotLightPos,
		spotLightRange,
		spotLightAttenuation,
		spotLightDir.getNorm(),
		spotLightExp,
		spotLightADS.Ambient,
		spotLightADS.Diffuse,
		spotLightADS.Specular);



	pShaderLightHouseRef->SetDirectionalLight(
		directionalLightRefDir.getNorm(),
		sunLightADS.Ambient,
		sunLightADS.Diffuse,
		sunLightADS.Specular);

	pShaderLightHouseRef->AddPointLight(
		lamp1LightPositionRef,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderLightHouseRef->AddPointLight(
		lamp2LightPositionRef,
		lampLightRange,
		lampLightAttenuation,
		lampLightADS.Ambient,
		lampLightADS.Diffuse,
		lampLightADS.Specular);

	pShaderLightHouse->AddSpotLight(
		spotLightPos,
		spotLightRange,
		spotLightAttenuation,
		spotLightDir.getNorm(),
		spotLightExp,
		spotLightADS.Ambient,
		spotLightADS.Diffuse,
		spotLightADS.Specular);

	pShaderTerrain->AddSpotLight(
		spotLightPos,
		spotLightRange,
		spotLightAttenuation,
		spotLightDir.getNorm(),
		spotLightExp,
		spotLightADS.Ambient,
		spotLightADS.Diffuse,
		spotLightADS.Specular);

	pShaderWater->AddSpotLight(
		spotLightPos,
		spotLightRange,
		spotLightAttenuation,
		spotLightDir.getNorm(),
		spotLightExp,
		spotLightADS.Ambient,
		spotLightADS.Diffuse,
		spotLightADS.Specular);

	// Initialize the projection matrix
	mCam.setPerspective(3.14159f / 3, mClientWidth / (float)mClientHeight, 1.0f, 1000.0f);
	mCam.setOrientAndPosition(Vect(0.f, 1.f, 0.f), Vect(0.f, 20.f, -10.f), Vect(-140.f, 110.f, -100.f));

	mTimer.Reset();
}

void DXApp::UpdateScene()
{
	// Define movement parameters
	float moveSpeed = 0.02f;
	float movementRange = 5.0f;
	float smoothnessFactor = 0.05f; 
	// Calculate smooth movement along X-axis
	float deltaX = sinf(smoothnessFactor) * movementRange * moveSpeed;
	// Create translation and rotation matrices
	Matrix translationMatrix = Matrix(TRANS, deltaX, 0.0f, 0.0f);

	//mWorldShip = mWorldShip * translationMatrix;
	//pShipObj->SetWorld(mWorldShip);

	//spotLightPos = mWorldShip.get(ROW_3);
	//spotLightPos.X() += 6.5f;
	//spotLightPos.Y() += 5.0f;

	////spotLightPos = Vect(0.f, 46.f, -22.8f);
	//spotLightDir = Vect(0.0f, -1.f, -5.5f, 0.0f) * Matrix(ROT_Y, cosf(mTimer.TotalTime() * 0.2f)) * Matrix(TRANS, spotLightPos);
	//spotLightDir.W() = 0.0f;
	spotLightPos = Vect(0.f, 85.f, -16.8f);
	spotLightDir = Vect(2.f, -1.f, -3.0f, 0.0f) /** Matrix(ROT_Y, cosf(mTimer.TotalTime() * 0.2f))*/;
	//spotLightDir.W() = 0.0f;

	pShaderTexLight->UpdateSpotLight(LightParam::POS, 0, spotLightPos);
	pShaderTexLight->UpdateSpotLight(LightParam::DIR, 0, spotLightDir);

	pShaderLightHouse->UpdateSpotLight(LightParam::POS, 0, spotLightPos);
	pShaderLightHouse->UpdateSpotLight(LightParam::DIR, 0, spotLightDir);

	pShaderTerrain->UpdateSpotLight(LightParam::POS, 0, spotLightPos);
	pShaderTerrain->UpdateSpotLight(LightParam::DIR, 0, spotLightDir.getNorm());

	pShaderWater->UpdateSpotLight(LightParam::POS, 0, spotLightPos);
	pShaderWater->UpdateSpotLight(LightParam::DIR, 0, spotLightDir.getNorm());

	//pShaderWater->SetWater(mTimer.TotalTime(), (float)mHeigtMapSize, mWaterYSize);

	//mCam.updateCamera();
	float camSpeed = 0.5f;
	if (GetKeyState('W') & 0x08000)
	{
		mCam.TranslateFwdBack(camSpeed);
	}
	else if (GetKeyState('S') & 0x08000)
	{
		mCam.TranslateFwdBack(-camSpeed);
	}

	if (GetKeyState('A') & 0x08000)
	{
		mCam.TranslateLeftRight(-camSpeed);
	}
	else if (GetKeyState('D') & 0x08000)
	{
		mCam.TranslateLeftRight(camSpeed);
	}

	float rotSpeed = 0.05f;
	if (GetKeyState(VK_LEFT) & 0x08000)
	{
		mCam.TurnLeftRight(rotSpeed);
	}
	else if (GetKeyState(VK_RIGHT) & 0x08000)
	{
		mCam.TurnLeftRight(-rotSpeed);
	}

	if (GetKeyState(VK_UP) & 0x08000)
	{
		mCam.TiltUpDown(rotSpeed);
	}
	else if (GetKeyState(VK_DOWN) & 0x08000)
	{
		mCam.TiltUpDown(-rotSpeed);
	}

	mCam.updateCamera();
}

void DXApp::DrawScene()
{
	const Matrix& ReflectionMat = pMirrorMan->GetReflectionMat();
	// Clear the back buffer
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, VasA(BackgroundColor));
	// ClearDepthStencilView clears both the depth and stencil components of the depth-stencil buffer.
	md3dImmediateContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->SkyboxRender(pSkyboxObj);

	// Render objects in _front_ of the mirror
	Vect eyepos;
	mCam.getPos(eyepos);
	Vect eyeposRef = eyepos * ReflectionMat;
	Matrix viewMat = mCam.getViewMatrix();
	Matrix projMat = mCam.getProjMatrix();


	pShaderTexLight->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	//pLightHouseObj->Render();
	pLampObj1->Render();
	pLampObj2->Render();

	pShaderLightHouse->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	pLightHouseObj->Render();

	//Render objects reflected _in_ the mirror.
	pMirrorMan->StartReflection(md3dImmediateContext, viewMat, projMat, eyepos, pSkyboxObjRef);

		pShaderTexLightRef->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyeposRef);
		//pLightHouseObjRef->Render();
		pLampObj1Ref->Render();
		pLampObj2Ref->Render();

		pShaderLightHouseRef->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyeposRef);
		pLightHouseObjRef->Render();

    pMirrorMan->EndReflection(md3dImmediateContext, viewMat, projMat, eyepos);
	pShaderTexLight->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);

	//// Mark the ship in the sea to prevent the sea surface from appearing inside the ship's geometry
	//md3dImmediateContext->OMSetDepthStencilState(MarkShipDSS, 1);
	//pShaderTexLight->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	//pShipObj->SetWorld(/*Matrix(SCALE, 0.95f, 0.95f, 0.95f) * */mWorldShip);
	//pShipObj->Render();
	//pShipObj->SetWorld(mWorldShip);
	//md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// Set the blend state for water and terrain
	md3dImmediateContext->OMSetBlendState(mWaterBlendEffect, nullptr, 0xFFFFFFFF);
	pShaderTerrain->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	pTerrainObj->Render();

	//pShaderTexLight->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	//pShipObj->Render();

	// Render the water surface to avoid overlapping the ship's geometry.
	//md3dImmediateContext->OMSetDepthStencilState(DrawWaterDSS, 1);
	pShaderWater->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	pShaderWater->SetWater(mTimer.TotalTime(), (float)mHeigtMapSize, mWaterYSize);
	pWaterObj->Render(); 

	// Back to normal pipeline stat
	md3dImmediateContext->OMSetBlendState(0, nullptr, 0xffffffff);
	//md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// Render objects _behind_ the mirror
	pShaderTexLight->StartShaderPass(md3dImmediateContext, viewMat, projMat, eyepos);
	pPlaneObj->Render();
	pSphereObj->Render();
	pCubeObj1->Render();
	pCubeObj2->Render();
	pPyramidObj->Render();
	//pBoatObj->Render();
	pShipObj->Render();

	pRockObj1->SetWorld(mWorldRock1);
	pRockObj1->Render();
	pRockObj1->SetWorld(Matrix(ROT_X, 3.1415f / 5.63f) * Matrix(ROT_Y, 3.1415f / 4.23f) * mWorldRock1 * Matrix(TRANS, -6.5f, 2.f, -5.5f));
	pRockObj1->Render();

	pRockObj2->Render();

	pRockObj3->SetWorld(mWorldRock3);
	pRockObj3->Render();

	pRockObj3->SetWorld(Matrix(ROT_Y, -3.1415f / 2.23f) * mWorldRock3 * Matrix(TRANS, 2.f, -0.4f, -9.9f));
	pRockObj3->Render();

	// Switches the display to show the now-finished back-buffer
	mSwapChain->Present(SyncInterval, 0);
}

DXApp::~DXApp()
{
	delete pShaderCol;
	delete pShaderTexLight;
	delete pShaderTexLightRef;
	delete pShaderLightHouse;
	delete pShaderLightHouseRef;
	delete pPlane;
	delete pCube;
	delete pMirror;
	delete pPlaneObj;
	delete pMirrorObj;
	delete pSkyboxModel;
	delete pSkyboxObj;
	delete pSkyboxObjRef;
	delete pShaderSkybox;
	delete pShaderSkyboxRef;
	delete pMirrorMan;
	delete pShaderTerrain;
	delete pTerrain;
	delete pTerrainObj;
	delete pSphere;
	delete pSphereObj;
	delete pLightHouse;
	delete pLightHouseObj;
	delete pLightHouseObjRef;
	//delete pBoat;
	//delete pBoatObj;
	delete pShaderWater;
	delete pWater;
	delete pWaterObj;
	delete pCubeObj1;
	delete pCubeObj2;
	delete pPyramid;
	delete pPyramidObj;
	delete pLamp;
	delete pLampObj1;
	delete pLampObj2;
	delete pLampObj1Ref;
	delete pLampObj2Ref;
	delete pShip;
	delete pShipObj;

	delete pRock1;
	delete pRockObj1;
	delete pRock2;
	delete pRockObj2;
	delete pRock3;
	delete pRockObj3;

	TEX::Destroy();

	ReleaseAndDeleteCOMobject(mRenderTargetView);
	ReleaseAndDeleteCOMobject(mpDepthStencilView);
	ReleaseAndDeleteCOMobject(mSwapChain);
	ReleaseAndDeleteCOMobject(md3dImmediateContext);
	ReleaseAndDeleteCOMobject(mSkyboxDesc);
	ReleaseAndDeleteCOMobject(mWaterBlendEffect);
	ReleaseAndDeleteCOMobject(MarkShipDSS);
	ReleaseAndDeleteCOMobject(DrawWaterDSS);

	// Must be done BEFORE the device is released
	ReportLiveDXObjects();		// See http://masterkenth.com/directx-leak-debugging/

	ReleaseAndDeleteCOMobject(md3dDevice);
}

// See http://masterkenth.com/directx-leak-debugging/
void DXApp::ReportLiveDXObjects()
{
#ifdef _DEBUG
	HRESULT hr = S_OK;

	// Now we set up the Debug interface, to be queried on shutdown
	ID3D11Debug* debugDev;
	hr = md3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));

	debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	ReleaseAndDeleteCOMobject(debugDev);
#endif
}

void DXApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << mspf << " (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void DXApp::FrameTick()
{
	mTimer.Tick();
	CalculateFrameStats();

	this->UpdateScene();
	this->DrawScene();
}

void DXApp::OnMouseDown(WPARAM btnState, int xval, int yval)
{
	UNREFERENCED_PARAMETER(btnState);
	UNREFERENCED_PARAMETER(xval);
	UNREFERENCED_PARAMETER(yval);
}

void DXApp::OnMouseUp(WPARAM btnState, int xval, int yval)
{
	UNREFERENCED_PARAMETER(btnState);
	UNREFERENCED_PARAMETER(xval);
	UNREFERENCED_PARAMETER(yval);
}

void DXApp::OnMouseMove(WPARAM btnState, int xval, int yval)
{
	UNREFERENCED_PARAMETER(btnState);
	UNREFERENCED_PARAMETER(xval);
	UNREFERENCED_PARAMETER(yval);
}

void DXApp::SkyboxRender(Skybox* pskyBox)
{
	if (pskyBox)
	{
		Vect eyepos;
		mCam.getPos(eyepos);
		Matrix viewMat = mCam.getViewMatrix();
		Matrix projMat = mCam.getProjMatrix();

		md3dImmediateContext->OMSetDepthStencilState(mSkyboxDesc, 0);
		Matrix skyboxViewMat = viewMat;
		skyboxViewMat.set(ROW_3, Vect(0.0f, 0.0f, 0.0f, 1.0f));
		pSkyboxObj->GetShader()->StartShaderPass(md3dImmediateContext, skyboxViewMat, projMat, eyepos);
		pSkyboxObj->Render();
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
	}
}