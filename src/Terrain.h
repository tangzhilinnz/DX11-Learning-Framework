#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "GraphicObject_Base.h"
#include "Vect.h"
#include "ShaderTerrain.h"

class Terrain : public GraphicObject_Base
{
public:
	Terrain(const Terrain&) = delete;				   // Copy constructor
	Terrain(Terrain&&) = default;                      // Move constructor
	Terrain& operator=(const Terrain&) & = default;    // Copy assignment operator
	Terrain& operator=(Terrain&&) & = default;         // Move assignment operator
	~Terrain();		  								   // Destructor

	Terrain() = delete;

	void SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp);
	void SetTexture(Texture* tex);
	void SetTexture(Texture* tex, int meshnum);
	void SetNormalMap(Texture* tex);
	void SetNormalMap(Texture* tex, int meshnum);

	void SetWorld(const Matrix& m);
	virtual void Render() override;

	Terrain(ShaderTerrain* shader, Model* terrainMod);

private:
	ShaderTerrain*	 pShader;
	Texture*         Textures[2];
	Texture*         NormalMaps[2];
	Matrix			 mWorld;
	Material         MeshMats;
};

#endif _TERRAIN_H_
