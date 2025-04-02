#ifndef SHADER_COLOR_OPTIMIZED_H
#define SHADER_COLOR_OPTIMIZED_H

#include "ShaderBase.h"
#include "Matrix.h"

struct ID3D11Buffer;
struct ID3D11Device;

class ShaderColorOptimized : public ShaderBase
{
public:
	ShaderColorOptimized(const ShaderColorOptimized&) = delete;
	ShaderColorOptimized(ShaderColorOptimized&&) = default;
	ShaderColorOptimized& operator=(const ShaderColorOptimized&) & = default;
	ShaderColorOptimized& operator=(ShaderColorOptimized&&) & = default;
	~ShaderColorOptimized();

	ShaderColorOptimized(ID3D11Device* device);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;

	void ShaderColorOptimized::SendWVPColor(const Matrix& WVP, const Vect& col);

private:
	struct Data_WVPColor
	{
		Matrix WVP; // World * View * Projection
		Vect Color;
	};

	ID3D11Buffer* mpBuffWVPColor;
};

#endif SHADER_COLOR_OPTIMIZED_H