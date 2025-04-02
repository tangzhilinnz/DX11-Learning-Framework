#ifndef _SHADER_LIGHT_H_
#define _SHADER_LIGHT_H_

#include "ShaderBase.h"
#include "Matrix.h"
#include <vector>

#define MAX_POINT_LIGHTS 20
#define MAX_SPOT_LIGHTS 20

struct ID3D11Buffer;
struct ID3D11Device;

enum class LightParam
{
	POS,         // Vect
	RANGE,       // float
	ATTE,        // Vect
	DIR,         // Vect
	SPOTEXP,     // float
	AMB,         // Vect
	DIFF,        // Vect
	SPEC         // Vect
};

class ShaderLight : public ShaderBase
{

public:
	ShaderLight(const ShaderLight&) = delete;				   // Copy constructor
	ShaderLight(ShaderLight&&) = default;                      // Move constructor
	ShaderLight& operator=(const 	ShaderLight&) & = default;  // Copy assignment operator
	ShaderLight& operator=(ShaderLight&&) & = default;         // Move assignment operator
	virtual ~ShaderLight();		  							   // Destructor

	ShaderLight(ID3D11Device* device, WCHAR* filename);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	void SetFog(
		int fogType,
		const Vect& fogColor,
		float fogStart,
		float fogRange,
		float fogThickness = FLT_MAX,
		float fogBaseHeight = 0.0f,
		float expFogDensity = 1.0f,
		float opacity = 1.0f);

	void SetDirectionalLight(
		const Vect& dir,
		const Vect& amb = Vect(1.f, 1.f, 1.f),
		const Vect& dif = Vect(1.f, 1.f, 1.f),
		const Vect& sp = Vect(1.f, 1.f, 1.f));

	void AddPointLight(
		const Vect& pos,
		float r,
		const Vect& att,
		const Vect& amb = Vect(1.f, 1.f, 1.f),
		const Vect& dif = Vect(1.f, 1.f, 1.f),
		const Vect& sp = Vect(1.f, 1.f, 1.f));

	void AddSpotLight(
		const Vect& pos, float r,
		const Vect& att, const Vect& dir,
		float spotExp,
		const Vect& amb = Vect(1.f, 1.f, 1.f),
		const Vect& dif = Vect(1.f, 1.f, 1.f),
		const Vect& sp = Vect(1.f, 1.f, 1.f));

	void UpdateDirectionalLight(LightParam Param, const Vect& value);

	void UpdatePointLight(LightParam Param, int index, const Vect& value);
	void UpdatePointLight(LightParam Param, int index, float value);

	void UpdateSpotLight(LightParam Param, int index, const Vect& value);
	void UpdateSpotLight(LightParam Param, int index, float value);

	void SendFogParameters();
	void SendCamMatrices(const Matrix& view, const Matrix& proj);
	void SendLightParameters(const Vect& eyepos);
	void SendWorldAndMaterial(
		const Matrix& world,
		const Vect& amb = Vect(.5f, .5f, .5f),
		const Vect& dif = Vect(.5f, .5f, .5f),
		const Vect& sp = Vect(.5f, .5f, .5f));

	virtual void StartShaderPass(ID3D11DeviceContext* devcon, const Matrix& view,
		const Matrix& proj, const Vect& eyepos) override;

protected:

	struct Material
	{
		Vect Ambient;
		Vect Diffuse;
		Vect Specular;
	};

	struct PhongADS
	{
		Vect Ambient;
		Vect Diffuse;
		Vect Specular;
	};

	struct Data_Fog
	{
		Vect  FogColor;               // Color of the fog
		float FogStart = 0.0f;       // Distance at which fog starts
		float FogRange = 0.0f;       // Range over which fog increases
		float FogThickness = 0.0f;   // Thickness of the fog layer (vertical extent)
		float FogBaseHeight = 0.0f;  // Base height of the fog layer (bottom of the fog)
		float ExpFogDensity = 1.0f;  // Density for exponential fog
		float Opacity = 1.0f;
		int FogType = 3;             // 0 = Linear, 1 = Exponential, 2 = Layered
	};

	Data_Fog FogPara;

	struct DirectionalLight
	{
		PhongADS Light;
		Vect Direction;
	};

	DirectionalLight DirLightData;

	struct PointLight
	{
		PhongADS Light;
		Vect Position;
		Vect Attenuation;
		float Range = 0.0f;
	};

	PointLight PointLightDatas[MAX_POINT_LIGHTS];

	struct SpotLight
	{
		PhongADS Light;
		Vect Position;
		Vect Attenuation;
		Vect Direction;
		float SpotExp = 0.0f;
		float Range = 0.0f;
	};

	SpotLight SpotLightDatas[MAX_SPOT_LIGHTS];

	struct CamMatrices
	{
		Matrix View;
		Matrix Projection;
	};

	ID3D11Buffer* mpBufferCamMatrices;

	struct Data_WorldAndMaterial
	{
		Matrix   World;
		Matrix   WorlInv;
		Material Mat;
	};

	ID3D11Buffer* mpBuffWordAndMaterial;

	struct Data_LightParams
	{
		DirectionalLight DirLight;
		Vect EyePosWorld;
		int NumPointLights = 0;             // Number of active point lights
		int NumSpotLights = 0;              // Number of active spotlights
	};

	ID3D11Buffer* mpBufferLightParams;

	ID3D11Buffer* mpBufferFog;          // Structured buffer for
	ID3D11Buffer* mpBufferPointLights;  // Structured buffer for point lights
	ID3D11Buffer* mpBufferSpotLights;   // Structured buffer for spotlights
	ID3D11ShaderResourceView* mpBufferPointLightsSRV; // SRV for point lights
	ID3D11ShaderResourceView* mpBufferSpotLightsSRV;  // SRV for spotlights
	int mNumPointLights;
	int mNumSpotLights;
};

#endif _SHADER_LIGHT_H_

