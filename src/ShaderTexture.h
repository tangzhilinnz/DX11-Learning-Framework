#ifndef _SHADER_TEXTURE_H_
#define _SHADER_TEXTURE_H_

#include "ShaderNullLight.h"
#include "Texture.h"
#include "Matrix.h"

struct ID3D11Buffer;
struct ID3D11Device;

class ShaderTexture : public ShaderNullLight
{
public:
	ShaderTexture(const ShaderTexture&) = delete;
	ShaderTexture(ShaderTexture&&) = default;
	ShaderTexture& operator=(const ShaderTexture&) & = default;
	ShaderTexture& operator=(ShaderTexture&&) & = default;
	~ShaderTexture();

	ShaderTexture(ID3D11Device* device);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	void SetTextureResourceAndSampler(Texture* tex);
	void SendWorld(const Matrix& world);

private:
	struct Data_World
	{
		Matrix World;
	};

	ID3D11Buffer* mpBuffWorld;
};

#endif _SHADER_TEXTURE_H_