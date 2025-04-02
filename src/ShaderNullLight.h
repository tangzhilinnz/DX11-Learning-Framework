#ifndef _SHADER_NULL_LIGHT_H_
#define _SHADER_NULL_LIGHT_H_

#include "ShaderBase.h"
#include "Matrix.h"
#include <vector>

struct ID3D11Buffer;
struct ID3D11Device;

class ShaderNullLight : public ShaderBase
{

public:
	ShaderNullLight(const ShaderNullLight&) = delete;				   // Copy constructor
	ShaderNullLight(ShaderNullLight&&) = default;                      // Move constructor
	ShaderNullLight& operator=(const 	ShaderNullLight&) & = default;  // Copy assignment operator
	ShaderNullLight& operator=(ShaderNullLight&&) & = default;         // Move assignment operator
	virtual ~ShaderNullLight();		  							   // Destructor

	ShaderNullLight(ID3D11Device* device, WCHAR* filename);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	void SetFog(
		int fogType,
		const Vect& fogColor,
		float fogStart,
		float fogRange,
		float fogThickness = FLT_MAX,
		float fogBaseHeight = 0.0f,
		float expFogDensity = 1.0f,
		float opcity = 1.0f);


	void SendFogParameters(const Vect& eyepos);
	void SendCamMatrices(const Matrix& view, const Matrix& proj);

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
		Vect  EyePosWorld;            // Eye position in world space
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

	struct CamMatrices
	{
		Matrix View;
		Matrix Projection;
	};

	ID3D11Buffer* mpBufferCamMatrices;
	ID3D11Buffer* mpBufferFog;
};

#endif _SHADER_NULL_LIGHT_H_