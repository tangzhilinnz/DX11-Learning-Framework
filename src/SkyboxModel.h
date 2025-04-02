#ifndef _SKYBOX_MODEL_H_
#define _SKYBOX_MODEL_H_

#include "Matrix.h"
#include "Model.h"

class Model;
struct ID3D11DeviceContext;
struct ID3D11Device;

class SkyboxModel : public Align16
{
private:
	Model* pUnitBox;

public:
	SkyboxModel(const SkyboxModel&) = delete;
	SkyboxModel(SkyboxModel&&) = delete;
	SkyboxModel& operator=(const SkyboxModel&) & = delete;
	SkyboxModel& operator=(SkyboxModel&&) & = delete;
	~SkyboxModel();

	SkyboxModel(ID3D11Device* dev, float scale);

	void Render(ID3D11DeviceContext* context);

	Model* GetModel()
	{
		return pUnitBox;
	}

private:
	void privCreateUnitBoxSixFacesInward(StandardVertex* pVerts, TriangleByIndex* pTriList, float scale);

};

#endif _SKYBOX_MODEL_H_