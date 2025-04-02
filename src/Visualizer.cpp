#include "Visualizer.h"
#include "Model.h"
#include "ShaderColor.h"
#include "Camera.h"
#include "d3dUtil.h"
#include "Camera.h"

Visualizer* Visualizer::pInstance = nullptr;

void Visualizer::Initialize(ID3D11Device* dev, ID3D11DeviceContext* con)
{
	pInstance = new	Visualizer(dev, con);
}

Visualizer::Visualizer(ID3D11Device* dev, ID3D11DeviceContext* con)
	: md3dContext(con), md3dDev(dev)
{
	pBoxModel = new Model(md3dDev, Model::UnitBox);
	pShaderCol = new ShaderColor(md3dDev);
}

Visualizer::~Visualizer()
{
	delete pBoxModel;
	delete pShaderCol;
}

void Visualizer::privVisualizeAll(Camera* cam)
{
	pShaderCol->SetToContext(md3dContext);
	pShaderCol->SendCamMatrices(cam->getViewMatrix(), cam->getProjMatrix());
	pBoxModel->SetToContext(md3dContext);

	for (auto it = RenderDataList.begin(); it != RenderDataList.end(); it++)
	{
		pShaderCol->SendWorldColor((*it)->world, (*it)->col);
		pBoxModel->Render(md3dContext);

		delete (*it);
	}

	RenderDataList.clear();

}

void Visualizer::Delete()
{
	delete pInstance;
}

void Visualizer::ShowMarker(const Vect& pos, const Vect& col, float size)
{
	Matrix world = Matrix(SCALE, size, size, size)* Matrix(TRANS, pos);
	Instance().RenderDataList.push_back(new RenderData(Matrix(world), col));
}

void Visualizer::ShowSegment(const Vect& start, const Vect& end, const Vect& col, float linewidth)
{
	Vect v = end - start;
	Vect up = Vect(0, 1, 0);
	if (abs(up.dot(v.getNorm())) == 1)
		up = Vect(1, 0, 0);

	Matrix world = Matrix(SCALE, linewidth, linewidth, v.mag()) 
					* Matrix(ROT_ORIENT, v, up)
					* Matrix(TRANS, .5f * (start+end));
	Instance().RenderDataList.push_back(new RenderData(Matrix(world), col));
}

void Visualizer::ShowWorldAxis(float size, float linewidth)
{
	ShowSegment(Vect(0, 0, 0), Vect(size, 0, 0), Colors::Red, linewidth);
	ShowSegment(Vect(0, 0, 0), Vect(0, size, 0), Colors::Green, linewidth);
	ShowSegment(Vect(0, 0, 0), Vect(0, 0, size), Colors::Cyan, linewidth);
}

void Visualizer::privShowNormal(const Vect& start, const Vect& end, const Vect& col, float size, float linewidth)
{
	ShowSegment(start, end, col, linewidth);
	ShowMarker(start, col, size / 20.0f);
}

void Visualizer::ShowVertNormals(Model* m, const Matrix& world, const Vect& col, float size, float linewidth)
{
	StandardVertex v;
	for (int i = 0; i < m->GetNumVert(); i++)
	{
		v = m->GetVertArray()[i];
		Instance().privShowNormal(v.Pos * world, v.Pos * world + size * (v.normal * world).getNorm(), col, size, linewidth);
	}
}

void Visualizer::ShowVertNormalsNearby(Camera& cam, float range, Model* m, const Matrix& world, const Vect& col, float size, float linewidth)
{
	Vect campos;
	cam.getPos(campos);
	Vect camdir;
	cam.getDir(camdir);

	StandardVertex v;
	float rngsq = range * range;
	for (int i = 0; i < m->GetNumVert(); i++)
	{
		v = m->GetVertArray()[i];

		Vect posWorld = v.Pos * world;

		Vect w = posWorld - campos;
		if  ( camdir.dot(w) < 0 && (w.magSqr() < rngsq)) 
		{
			Instance().privShowNormal(posWorld, posWorld + size * (v.normal * world).getNorm(), col, size, linewidth);
		}
	}
}
