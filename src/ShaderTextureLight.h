#ifndef _SHADER_TEXTURE_LIGHT_H_
#define _SHADER_TEXTURE_LIGHT_H_

#include "ShaderLight.h"
#include "Matrix.h"
#include "TextureManager.h"

class ShaderTextureLight : public ShaderLight
{
public:
	ShaderTextureLight(const ShaderTextureLight&) = delete;				   // Copy constructor
	ShaderTextureLight(ShaderTextureLight&&) = default;                    // Move constructor
	ShaderTextureLight& operator=(const ShaderTextureLight&) & = default;  // Copy assignment operator
	ShaderTextureLight& operator=(ShaderTextureLight&&) & = default;       // Move assignment operator
	~ShaderTextureLight();		  							               // Destructor

	ShaderTextureLight(ID3D11Device* device, WCHAR* filename = L"../Assets/Shaders/TextureLight.hlsl");
	virtual void SetToContext(ID3D11DeviceContext* devcon) override;
	void SetTextureResourceAndSampler(Texture* tex, Texture* normalMap = nullptr);

private:
	Texture* pDNM = nullptr;
};

#endif _SHADER_TEXTURE_LIGHT_H_

