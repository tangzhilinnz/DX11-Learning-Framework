#ifndef _SHADER_COLOR_LIGHT_H_
#define _SHADER_COLOR_LIGHT_H_

#include "ShaderLight.h"
#include "Matrix.h"

class ShaderColorLight : public ShaderLight
{

public:
	ShaderColorLight(const ShaderColorLight&) = delete;				   // Copy constructor
	ShaderColorLight(ShaderColorLight&&) = default;                    // Move constructor
	ShaderColorLight& operator=(const ShaderColorLight&) & = default;  // Copy assignment operator
	ShaderColorLight& operator=(ShaderColorLight&&) & = default;       // Move assignment operator
	~ShaderColorLight();		  							           // Destructor

	ShaderColorLight(ID3D11Device* device, bool isToon = false);
	virtual void SetToContext(ID3D11DeviceContext* devcon) override;
};

#endif _SHADER_COLOR_LIGHT_H_