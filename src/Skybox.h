#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "GraphicObject_Base.h"
#include "Vect.h"
#include "ShaderTexture.h"

class Skybox : public GraphicObject_Base
{
public:
	Skybox(const Skybox&) = delete;				     // Copy constructor
	Skybox(Skybox&&) = default;                      // Move constructor
	Skybox& operator=(const Skybox&) & = default;    // Copy assignment operator
	Skybox& operator=(Skybox&&) & = default;         // Move assignment operator
	~Skybox();		  								 // Destructor

	Skybox() = delete;

	void SetTexture(Texture* tex);

	ShaderTexture* GetShader()
	{
		return this->pShader;
	}

	void SetWorld(const Matrix& m);
	virtual void Render() override;

	Skybox(ShaderTexture* shader, Model* skyboxModel);

private:
	ShaderTexture*   pShader;
	Texture*         pTexture;
	Matrix			 mWorld;
};

#endif _SKYBOX_H_