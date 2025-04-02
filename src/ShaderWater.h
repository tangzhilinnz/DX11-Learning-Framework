#ifndef _SHADER_WATER_H_
#define _SHADER_WATER_H_

#include "ShaderLight.h"
#include "Matrix.h"
#include "Texture.h"

class ShaderWater : public ShaderLight
{

public:
	ShaderWater(const ShaderWater&) = delete;				   // Copy constructor
	ShaderWater(ShaderWater&&) = default;                    // Move constructor
	ShaderWater& operator=(const ShaderWater&) & = default;  // Copy assignment operator
	ShaderWater& operator=(ShaderWater&&) & = default;       // Move assignment operator
	~ShaderWater();		  							           // Destructor

	ShaderWater(ID3D11Device* device);
	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	void SetWater(float time, float heightMapSize, float waterSizeY);
	void SetTextureResourceAndSampler(Texture* heightMap);

	void SendWaterParameters();

	virtual void StartShaderPass(ID3D11DeviceContext* devcon, const Matrix& view,
		const Matrix& proj, const Vect& eyepos) override;

private:
	struct Data_Water
	{
		float time = 0.0f; // Time for animation
		float heightMapSize = 0.0f;
		float waterSizeY = 0.0f;
		float padding = 0.0f;
	};

	Data_Water WaterPara;

	ID3D11Buffer* mpBufferWater;        // Structured buffer for
};

#endif _SHADER_WATER_H_