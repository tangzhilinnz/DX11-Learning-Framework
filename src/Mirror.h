#ifndef _MIRROR_H_
#define _MIRROR_H_

#include "Matrix.h"
#include <vector>
#include "GraphicObject_Color.h"
#include "Skybox.h"


struct ID3D11Buffer;
struct ID3D11Device;


class Mirror
{
public:
	Mirror(const Mirror&) = delete;				   // Copy constructor
	Mirror(Mirror&&) = default;                      // Move constructor
	Mirror& operator=(const 	Mirror&) & = default;  // Copy assignment operator
	Mirror& operator=(Mirror&&) & = default;         // Move assignment operator
	virtual ~Mirror();		  							   // Destructor

	Mirror(ID3D11Device* device);

	void StartReflection(ID3D11DeviceContext* devcon, const Matrix& viewMat,
		const Matrix& projMat, const Vect& eyepos, Skybox* skybox = nullptr);

	void EndReflection(ID3D11DeviceContext* devcon, const Matrix& viewMat,
		const Matrix& projMat, const Vect& eyepos);

	void SetMirrorMat(const Matrix& mirrorMat);
	void SetMirrorObj(GraphicObject_Color* mirrorObj);
	GraphicObject_Color* GetMirrorObj();
	const Matrix& GetReflectionMat() const;
	Matrix GetMirrorMat() const;

private:
	void priComputeReflectionMat(const Matrix& mirrorMat);

private:

	GraphicObject_Color* pMirrorObj;
	Matrix mReflectionMat;

	// Store these pointers to avoid passing them in every method
	ID3D11Device* mDevice;

	ID3D11DepthStencilState* MarkMirrorDSS;
	ID3D11DepthStencilState* DrawReflectionDSS;
	ID3D11DepthStencilState* DrawReflectionSkyboxDSS;

	ID3D11RasterizerState* FrontFaceAsCCWRS;
	ID3D11RasterizerState* MirrorFrontFaceAsClockWiseRS;

	ID3D11BlendState* NoWriteToRenderTargetBS;
	ID3D11BlendState* TransparentBS;
};

#endif _MIRROR_H_

