#ifndef _GRAPHIC_OBJECT_TEXTURE_Light_H_
#define _GRAPHIC_OBJECT_TEXTURE_Light_H_

#include "GraphicObject_Base.h"
#include "Vect.h"
#include "ShaderTextureLight.h"

class GraphicObject_TextureLight : public GraphicObject_Base
{
public:
	GraphicObject_TextureLight(const GraphicObject_TextureLight&) = delete;				   // Copy constructor
	GraphicObject_TextureLight(GraphicObject_TextureLight&&) = default;                    // Move constructor
	GraphicObject_TextureLight& operator=(const GraphicObject_TextureLight&) & = default;  // Copy assignment operator
	GraphicObject_TextureLight& operator=(GraphicObject_TextureLight&&) & = default;       // Move assignment operator
	~GraphicObject_TextureLight();		  											       // Destructor

	GraphicObject_TextureLight() = delete;

	void SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp);
	void SetMaterial(const Vect& amb, const Vect& dif, const Vect& sp, int meshnum);
	void SetTexture(Texture* tex);
	void SetTexture(Texture* tex, int meshnum);
	void SetNormalMap(Texture* tex);
	void SetNormalMap(Texture* tex, int meshnum);

	void SetWorld(const Matrix& m);
	virtual void Render() override;

	GraphicObject_TextureLight(ShaderTextureLight* shader, Model* mod);

private:
	ShaderTextureLight*	 pShader;
	Texture**            Textures;
	Texture**            NormalMaps;
	Matrix				 mWorld;
	Material*            MeshMats;
};

#endif _GRAPHIC_OBJECT_TEXTURE_Light_H_
