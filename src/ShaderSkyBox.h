#ifndef SHADER_SKYBOX_H
#define SHADER_SKYBOX_H

#include "ShaderBase.h"
#include "Texture.h"
#include "Matrix.h"

struct ID3D11Buffer;
struct ID3D11Device;

class ShaderSkyBox : public ShaderBase
{
public:
	ShaderSkyBox(const ShaderSkyBox&) = delete;
	ShaderSkyBox(ShaderSkyBox&&) = default;
	ShaderSkyBox& operator=(const ShaderSkyBox&) & = default;
	ShaderSkyBox& operator=(ShaderSkyBox&&) & = default;
	~ShaderSkyBox();

	ShaderSkyBox(ID3D11Device* device);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	void SetTextureResourceAndSampler(Texture* tex);
	void SendCamMatrices(const Matrix& view, const Matrix& proj);

private:
	struct CamMatrices
	{
		Matrix View;
		Matrix Projection;
	};

	ID3D11Buffer* mpBufferCamMatrices;
	Matrix mCamInitTrans;
};

#endif SHADER_SKYBOX_H