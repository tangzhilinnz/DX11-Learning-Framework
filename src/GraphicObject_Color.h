// GraphicObject_Color
// Andre Berthiaume, July 2016

#ifndef _GRAPHIC_OBJECT_COLOR_H_
#define _GRAPHIC_OBJECT_COLOR_H_

#include "GraphicObject_Base.h"
#include "Vect.h"
#include "ShaderColor.h"

class Mirror;

class GraphicObject_Color : public GraphicObject_Base
{
public:
	GraphicObject_Color(const GraphicObject_Color&) = delete;				 // Copy constructor
	GraphicObject_Color(GraphicObject_Color&&) = default;                    // Move constructor
	GraphicObject_Color& operator=(const GraphicObject_Color&) & = default;  // Copy assignment operator
	GraphicObject_Color& operator=(GraphicObject_Color&&) & = default;       // Move assignment operator
	~GraphicObject_Color();		  											 // Destructor

	GraphicObject_Color() = delete;

	void SetColor(const Vect& col);
	void SetColor(const Vect& col, int meshnum);
	void SetWorld(const Matrix& m);
	virtual void Render() override;

	ShaderColor* GetShader()
	{
		return this->pShader;
	}

	Matrix GetWorldMatrix()
	{
		return this->World;
	}

	GraphicObject_Color(ShaderColor* shader, Model* mod);

private:
	ShaderColor*	pShader;
	Vect*           MeshColors;
	Matrix			World;

};

#endif _GRAPHIC_OBJECT_COLOR_H_
