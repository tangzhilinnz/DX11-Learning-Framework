#ifndef _TERRAIN_MODEL_H_
#define _TERRAIN_MODEL_H_

#include "Matrix.h"
#include <tuple>
#include <d3d11.h>
#include "DirectXTex.h"

class Model;
struct ID3D11DeviceContext;
struct ID3D11Device;

class TerrainModel : public Align16
{

private:
	Model* pModTerrain;

public:
	TerrainModel(const TerrainModel&) = delete;
	TerrainModel(TerrainModel&&) = delete;
	TerrainModel& operator=(const TerrainModel&) & = delete;
	TerrainModel& operator=(TerrainModel&&) & = delete;
	~TerrainModel();

public:
	TerrainModel(ID3D11Device* dev, LPCWSTR heightmapFile,
		float len, float maxheight, float ytrans, int RepeatU, int RepeatV, int* pGetSize = nullptr);

	void Render(ID3D11DeviceContext* context);

	Model*  GetModel()
	{
		return pModTerrain;
	}

private:
	int TexelIndex(int channel, int side, int x, int y) const;

};


#endif _TERRAIN_MODEL_H_