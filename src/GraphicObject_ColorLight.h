#ifndef _GRAPHIC_OBJECT_COLOR_LIGHT_H_
#define _GRAPHIC_OBJECT_COLOR_LIGHT_H_

#include "GraphicObject_Base.h"
#include "Vect.h"
#include "ShaderColorLight.h"

class GraphicObject_ColorLight : public GraphicObject_Base
{
public:
	GraphicObject_ColorLight(const GraphicObject_ColorLight&) = delete;				   // Copy constructor
	GraphicObject_ColorLight(GraphicObject_ColorLight&&) = default;                    // Move constructor
	GraphicObject_ColorLight& operator=(const GraphicObject_ColorLight&) & = default;  // Copy assignment operator
	GraphicObject_ColorLight& operator=(GraphicObject_ColorLight&&) & = default;       // Move assignment operator
	~GraphicObject_ColorLight();		  											   // Destructor

	GraphicObject_ColorLight() = delete;

	void SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp);
	void SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp, int meshnum);
	void SetWorld(const Matrix& m);
	virtual void Render() override;

	GraphicObject_ColorLight(ShaderColorLight* shader, Model* mod);

private:
	ShaderColorLight*				pShader;
	Matrix							mWorld;
	Material*                       MeshMats;
};

#endif _GRAPHIC_OBJECT_COLOR_LIGHT_H_
