#include "TerrainModel.h"
#include "Model.h"
#include "d3dUtil.h"
#include "DirectXTex.h"
#include <assert.h>
#include "ModelTools.h"


int TerrainModel::TexelIndex(int channel, int side, int x, int z) const
{
	return channel * (z * side + x);
}

TerrainModel::~TerrainModel()
{
	delete pModTerrain;
}

TerrainModel::TerrainModel(ID3D11Device* dev, LPCWSTR heightmapFile, float len,
	float maxheight, float ytrans, int RepeatU, int RepeatV, int* pGetSize)
{
	DirectX::ScratchImage scrtTex;
	HRESULT hr = LoadFromTGAFile(heightmapFile, nullptr, scrtTex);
	assert(SUCCEEDED(hr));

	// Get metadata of the loaded texture
	const DirectX::TexMetadata& metadata = scrtTex.GetMetadata();
	int channel = 4;

	// Check the DXGI format
	if (metadata.format == DXGI_FORMAT_B8G8R8X8_UNORM ||
		metadata.format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
	{
		channel = 3;
	}

	if (metadata.format == DXGI_FORMAT_R8_UNORM ||
		metadata.format == DXGI_FORMAT_R8_UINT)
	{
		channel = 1;
	}

	const DirectX::Image* hgtmap = scrtTex.GetImage(0, 0, 0);
	assert(hgtmap->height > 1 && hgtmap->height == hgtmap->width);

	if (pGetSize)
	{
		*pGetSize = (int)hgtmap->height;
	}

	size_t side = hgtmap->height;	// the image should be square

	size_t nverts = side * side;
	StandardVertex* pVerts = new StandardVertex[nverts];

	size_t ntri = 2 * (side - 1) * (side - 1);
	TriangleByIndex* pTriList = new TriangleByIndex[ntri];

	float triangle_side = (float)len / (side - 1);

	// Fill vertex data
	for (size_t z = 0; z < side; z++) {
		for (size_t x = 0; x < side; x++) {
			// Extract height from heightmap from the grayscale image
			uint8_t h_val = hgtmap->pixels[TexelIndex(channel, side, x, z)];   // 'R' channel from RGBA
			float height = (h_val / 255.0f) * maxheight + ytrans;     // Normalize to [0, maxheight]

			pVerts[z * side + x].set(
				-0.5f * len + x * triangle_side,  // X Position
				height,                           // Y Position (height)
				-0.5f * len + z * triangle_side,  // Z Position
				(float)x / (side - 1) * RepeatU,  // U Texture Coordinate
				(float)z / (side - 1) * RepeatV   // V Texture Coordinate
			);
		}
	}

	for (size_t z = 0; z < side - 1; z++)
		for (size_t x = 0; x < side - 1; x++)
		{
			size_t i = (z * (side - 1) + x) * 2;
			size_t topLeft = z * side + x;
			size_t topRight = topLeft + 1;
			size_t bottomLeft = topLeft + side;
			size_t bottomRight = bottomLeft + 1;

			// First triangle
			pTriList[i].set(topRight, topLeft, bottomLeft);
			// Second triangle
			pTriList[i + 1].set(topRight, bottomLeft, bottomRight);
		}

	// Initialize vertex normals to zero
	for (size_t i = 0; i < nverts; i++)
	{
		pVerts[i].normal = Vect(0.0f, 0.0f, 0.0f, 0.0f);
	}

	// For each triangle in the mesh:
	for (size_t i = 0; i < ntri; i++)
	{
		// Indices of the ith triangle
		unsigned int i0 = pTriList[i].v0;
		unsigned int i1 = pTriList[i].v1;
		unsigned int i2 = pTriList[i].v2;

		// Compute edge vectors
		Vect e0 = pVerts[i1].Pos - pVerts[i0].Pos;
		Vect e1 = pVerts[i2].Pos - pVerts[i0].Pos;

		// Compute face normal
		Vect faceNormal = e0.cross(e1);

		float area = faceNormal.mag() * 0.5f; // Triangle area

		//pVerts[i0].normal += faceNormal;
		//pVerts[i1].normal += faceNormal;
		//pVerts[i2].normal += faceNormal;

		// Weight the normal by triangle area before accumulation
		pVerts[i0].normal += faceNormal * area;
		pVerts[i1].normal += faceNormal * area;
		pVerts[i2].normal += faceNormal * area;
	}

	// Normalize the accumulated vertex normals
	for (size_t i = 0; i < nverts; i++)
	{
		pVerts[i].normal.norm();
		pVerts[i].normal.W() = 0.0f;
	}

	int numverts = (int)nverts;
	int numtri = (int)ntri;

	ModelTools::ComputeTangent(pVerts, numverts, pTriList, numtri, true);

	pModTerrain = new Model(dev, pVerts, (int)nverts, pTriList, (int)ntri);

	delete[] pVerts;
	delete[] pTriList;
}

void TerrainModel::Render(ID3D11DeviceContext* context)
{
	pModTerrain->SetToContext(context);
	pModTerrain->Render(context);
}