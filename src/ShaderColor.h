#ifndef _SHADER_COLOR_H_
#define _SHADER_COLOR_H_

#include "ShaderNullLight.h"
#include "Matrix.h"

struct ID3D11Buffer;
struct ID3D11Device;

class ShaderColor : public ShaderNullLight
{
public:
	ShaderColor(const ShaderColor&) = delete;				 // Copy constructor
	ShaderColor(ShaderColor&&) = default;                    // Move constructor
	ShaderColor& operator=(const ShaderColor&) & = default;  // Copy assignment operator
	ShaderColor& operator=(ShaderColor&&) & = default;       // Move assignment operator
	~ShaderColor();		  							         // Destructor

	ShaderColor(ID3D11Device* device);

	virtual void SetToContext(ID3D11DeviceContext* devcon) override;
	void SendWorldColor(const Matrix& world, const Vect& col);

	struct Data_WorldColor
	{
		Matrix World;
		Vect Color;
	};

	ID3D11Buffer* mpBuffWorldColor;
};

#endif _SHADER_COLOR_H_
