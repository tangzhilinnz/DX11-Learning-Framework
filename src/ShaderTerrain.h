#ifndef _SHADER_TERRAIN_H_
#define _SHADER_TERRAIN_H_

#include "ShaderLight.h"
#include "Matrix.h"
#include "TextureManager.h"

class ShaderTerrain : public ShaderLight
{

public:
	ShaderTerrain(const ShaderTerrain&) = delete;				   // Copy constructor
	ShaderTerrain(ShaderTerrain&&) = default;                      // Move constructor
	ShaderTerrain& operator=(const ShaderTerrain&) & = default;    // Copy assignment operator
	ShaderTerrain& operator=(ShaderTerrain&&) & = default;         // Move assignment operator
	~ShaderTerrain();		  							           // Destructor

	ShaderTerrain(ID3D11Device* device);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	//void SetTextureResourceAndSampler(Texture* tex, Texture* tex2, Texture* tex3);
	void SetTextureResourceAndSampler(Texture* tex1, Texture* tex2,
		Texture* normalMap1 = nullptr, Texture* normalMap2 = nullptr);

private:
	Texture* pDNM = nullptr;

};

#endif _SHADER_TERRAIN_H_