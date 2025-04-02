#ifndef _WATER_H_
#define _WATER_H_

#include "GraphicObject_Base.h"
#include "Vect.h"
#include "ShaderWater.h"

class Water : public GraphicObject_Base
{
public:
	Water(const Water&) = delete;				   // Copy constructor
	Water(Water&&) = default;                    // Move constructor
	Water& operator=(const Water&) & = default;  // Copy assignment operator
	Water& operator=(Water&&) & = default;       // Move assignment operator
	~Water();		  											   // Destructor

	Water() = delete;

	void SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp);
	void SetWorld(const Matrix& m);
	void SetHeighMapTexture(Texture* heightMap);
	virtual void Render() override;

	Water(ShaderWater* shader, Model* mod);

private:
	ShaderWater*				    pShader;
	Matrix							mWorld;
	Material                        mMeshMat;
	Texture*                        pHeightMap;
};

#endif _WATER_H_
