// GraphicObject
// Andre Berthiaume, July 2016

#ifndef _GRAPHIC_OBJECT_BASE_H_
#define _GRAPHIC_OBJECT_BASE_H_

#include "Matrix.h"

class Model;
class ShaderMeshData;
struct ID3D11DeviceContext;
class ShaderBase;


struct Material
{
	Vect Ambient;
	Vect Diffuse;
	Vect Specular;
};

class GraphicObject_Base : public Align16
{
public:
	GraphicObject_Base(const GraphicObject_Base&) = delete;				   // Copy constructor
	GraphicObject_Base(GraphicObject_Base&&) = default;                    // Move constructor
	GraphicObject_Base& operator=(const GraphicObject_Base&) & = default;  // Copy assignment operator
	GraphicObject_Base& operator=(GraphicObject_Base&&) & = default;       // Move assignment operator
	~GraphicObject_Base() = default;		  							   // Destructor
	GraphicObject_Base();

	void SetModel(Model* mod);
	virtual void Render() = 0;

protected:
	Model* pModel;

};

#endif _GRAPHIC_OBJECT_BASE_H_
