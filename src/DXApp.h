// DXApp
// Andre Berthiaume, June 2016
// Note: DX 11 SDK https://www.microsoft.com/en-us/download/details.aspx?id=6812
// Note on weird stuff with swap chain (1s and 2s) https://msdn.microsoft.com/en-us/library/windows/desktop/jj863687(v=vs.85).aspx

#ifndef _DXApp_H_
#define _DXApp_H_

#include <d3d11.h>
#include "d3dUtil.h"
#include "Align16.h"
#include "GameTimer.h"
#include <string>

// New includes for demo
#include "Vect.h"
#include "Matrix.h"
#include "Camera.h"
#include "ShaderColor.h"
#include "ShaderTexture.h"
#include "ShaderColorLight.h"
#include "ShaderTextureLight.h"
#include "ShaderTerrain.h"
#include "ShaderWater.h"
#include "TextureManager.h"

class SkyboxModel;
class Model;
class FlatPlane;
class TerrainModel;
class GraphicObject_ColorLight;
class GraphicObject_Color;
class GraphicObject_TextureLight;
class GraphicObject_Texture;
class Terrain;
class Skybox;
class Mirror;
class Water;

class DXApp : public Align16
{
private:
	struct PhongADS
	{
		Vect Ambient;
		Vect Diffuse;
		Vect Specular;
	};

	// Main window handle
	HWND      mhMainWnd;

	Vect BackgroundColor;
	int SyncInterval = 1;  // 1 for sync to monitor refresh rate, 0 for no sync

	// DX application elements
	ID3D11Device* md3dDevice;					// Connects to the graphics card
	ID3D11DeviceContext* md3dImmediateContext;	// Settings for the GPU to use
	IDXGISwapChain* mSwapChain;					// image buffers used for rendering
	ID3D11RenderTargetView* mRenderTargetView;	// Where to send rendring operations (typically: points to one of the swap buffers)
	ID3D11DepthStencilView* mpDepthStencilView; // Needed to force depth-buffer operations
	ID3D11DepthStencilState* mSkyboxDesc;
	ID3D11BlendState* mWaterBlendEffect;
	ID3D11DepthStencilState* MarkShipDSS;
	ID3D11DepthStencilState* DrawWaterDSS;

	GameTimer mTimer;
	std::string mMainWndCaption;
	int mClientWidth;
	int mClientHeight;

	void InitDirect3D();
	void InitDemo();
	void UpdateScene();
	void DrawScene();
	void CalculateFrameStats();

	void SkyboxRender(Skybox* pskyBox = nullptr);

	// Debug utility
	void ReportLiveDXObjects();

	// Demo specific additions
	Camera mCam;

	ShaderColor* pShaderCol;
	ShaderTextureLight* pShaderTexLight;
	ShaderTextureLight* pShaderTexLightRef;


	ShaderTextureLight* pShaderLightHouse;
	ShaderTextureLight* pShaderLightHouseRef;

	Vect spotLightPos;
	FlatPlane* pMirror;
	GraphicObject_Color* pMirrorObj;
	Matrix mirrorMat;

	SkyboxModel* pSkyboxModel;
	Skybox* pSkyboxObj;
	Skybox* pSkyboxObjRef;
	ShaderTexture* pShaderSkybox;
	ShaderTexture* pShaderSkyboxRef;
	Texture* pSkyboxTex;
	Matrix mSkyboxMat;
	Matrix mSkyboxMatRef;

	Mirror* pMirrorMan;

	ShaderTerrain* pShaderTerrain;
	TerrainModel* pTerrain;
	Matrix mTerrainMat;
	Terrain* pTerrainObj;
	Texture* rockTex;
	Texture* cliffTex;
	Texture* rockTexNM;
	Texture* cliffTexNM;

	FlatPlane* pPlane;
	Matrix planeMat;
	GraphicObject_TextureLight* pPlaneObj;

	Model* pSphere;
	GraphicObject_TextureLight* pSphereObj;
	Matrix mSphereMat;
	Texture* sphereTex;
	Texture* sphereTexNM;

	Model* pCube;
	Matrix mCubeMat1;
	Matrix mCubeMat2;
	GraphicObject_TextureLight* pCubeObj1;
	GraphicObject_TextureLight* pCubeObj2;

	Model* pPyramid;
	Matrix mPyramidMat;
	GraphicObject_TextureLight* pPyramidObj;

	Model* pLightHouse;
	Texture* pLightHouseTex0;
	Texture* pLightHouseTex1;
	Texture* pLightHouseTex0NM;
	Texture* pLightHouseTex1NM;
	Matrix mWorldLightHouse;
	Matrix mWorldLightHouseRef;
	GraphicObject_TextureLight* pLightHouseObj;
	GraphicObject_TextureLight* pLightHouseObjRef;

	//Model* pBoat;
	//Texture* pBoatTex;
	//Texture* pBoatTexNM;
	//Matrix mWorldBoat;
	//GraphicObject_TextureLight* pBoatObj;

	ShaderWater* pShaderWater;
	TerrainModel* pWater;
	Matrix mWaterMat;
	Water* pWaterObj;
	Texture* waterHeightMap;
	int mHeigtMapSize;
	float mWaterXZSize;
	float mWaterYSize;

	PhongADS sunLightADS;
	Vect sunLightDir;
	Vect sunLightRefDir;

	PhongADS lampLightADS;
	Vect lamp1LightPosition;
    Vect lamp2LightPosition;
	Vect lamp1LightPositionRef;
	Vect lamp2LightPositionRef;
	Vect lampLightAttenuation;
	float lampLightRange = 0.0f;

	PhongADS spotLightADS;
	Vect spotLightAttenuation;
	Vect spotLightDir;
	float spotLightExp = 0.0f;
	float spotLightRange = 0.0f;

	Model* pLamp;
	Texture* pLampTex;
	Texture* pLampTexNM;
	Matrix mWorldLamp1;
	Matrix mWorldLamp2;
	Matrix mWorldLamp1Ref;
	Matrix mWorldLamp2Ref;
	GraphicObject_TextureLight* pLampObj1;
	GraphicObject_TextureLight* pLampObj2;
	GraphicObject_TextureLight* pLampObj1Ref;
	GraphicObject_TextureLight* pLampObj2Ref;

	Model* pShip;
	Texture* pShipTex1;
	Texture* pShipTex1NM;
	Matrix mWorldShip;
	GraphicObject_TextureLight* pShipObj;

	Model* pRock1;
	Texture* pRockTex1;
	Texture* pRockTex1NM;
	Matrix mWorldRock1;
	GraphicObject_TextureLight* pRockObj1;

	Model* pRock2;
	Texture* pRockTex2;
	Texture* pRockTex2NM;
	Matrix mWorldRock2;
	GraphicObject_TextureLight* pRockObj2;

	Model* pRock3;
	Texture* pRockTex3;
	Texture* pRockTex3NM;
	Matrix mWorldRock3;
	GraphicObject_TextureLight* pRockObj3;

public:
	DXApp(HWND hwnd);
	virtual ~DXApp();

	void FrameTick();

	// overrides for handling mouse input.
	void OnMouseDown(WPARAM btnState, int xval, int yval);
	void OnMouseUp(WPARAM btnState, int xval, int yval);
	void OnMouseMove(WPARAM btnState, int xval, int yval);
};

#endif _DXApp_H_