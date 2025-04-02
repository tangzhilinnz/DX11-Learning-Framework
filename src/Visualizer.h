// Visualizer
// AB 03/24

#ifndef _VISUALIZER_H_
#define _VISUALIZER_H_

#include "Vect.h"
#include "Matrix.h"
#include <list>
class Model;
class ShaderColor;
struct ID3D11Device;
struct ID3D11DeviceContext;
class Camera;

class Visualizer
{
private:

	struct RenderData 
	{
		Matrix world;
		Vect col;

		RenderData(Matrix m, Vect c)
			:world(m), col(c) {}

		RenderData()
			:RenderData(Matrix(IDENTITY), Vect(1, 1, 1)) {}
	};

	std::list<RenderData*> RenderDataList;

	static Visualizer* pInstance;

	Visualizer() = delete;
	Visualizer(ID3D11Device* dev, ID3D11DeviceContext* con);
	~Visualizer();
	Visualizer(const Visualizer&) = delete;
	Visualizer& operator=(const Visualizer&) = delete;

	static Visualizer& Instance()
	{
		assert((pInstance != nullptr) && "Call Vizualizer::Initialize in InitDemo!");
		return *pInstance;
	}

	ID3D11Device* md3dDev;					
	ID3D11DeviceContext* md3dContext;
	static Vect DEFAULT_COLOR;

	Model* pBoxModel;
	ShaderColor* pShaderCol;

	void privVisualizeAll(Camera* cam);

	void privShowNormal(const Vect& start, const Vect& end, const Vect& col, float size  = 1.0f, float linewidth = 0.01f);

public:
	static void Initialize(ID3D11Device* dev, ID3D11DeviceContext* con);
	static void ShowMarker(const Vect& pos, const Vect& col, float size = 1.0f);
	static void ShowSegment(const Vect& start, const Vect& end, const Vect& col, float linewidth = 0.01f);
	static void ShowWorldAxis(float size = 1.0f, float linewidth = 0.01f);
	static void ShowVertNormals(Model* m, const Matrix& world, const Vect& col, float size = 1, float linewidth = 0.01f);
	static void Visualizer::ShowVertNormalsNearby(Camera& cam, float range, Model* m, const Matrix& world, const Vect& col, float size = 1, float linewidth = 0.01f);

	static void Delete();
	static void VisualizeAll(Camera* cam) { pInstance->privVisualizeAll(cam); };
};


#endif _VISUALIZER_H_
