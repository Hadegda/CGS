#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <DirectXMath.h>
#include <unordered_map>
#include <vector>
#include "md2Model.h"
#include "GeneticAlgorithm.h"

struct cbTransform
{
	DirectX::XMMATRIX mTransform;
	DirectX::XMMATRIX mViewProj;
};

class Renderer {
public:
	bool Init(float alpha, float beta, float r);
	void Draw();
	
	bool LoadModel(wchar_t* path) noexcept;
	bool UnloadModel() noexcept;
	ModelMD2 * GetModel();
	GeneticAlgorithm* GetGA();

	Renderer();
	~Renderer();

	void SetSolidRS();
	void SetWireFrameRS();
	void ChangeViewMatrix(float alpha, float beta, float r);

	bool drawModel;
	bool drawSkelet;
	bool drawLines;
	bool drawBBox;
	bool drawResSkelet;
	bool isWireFrame;
	bool isPlaying;

private:
	bool CreateModelsNormals();

	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	ID3D11Buffer* pTransformCB;

	float ClearColor[4];

	ID3D11RasterizerState* pWireFrameRS;
	ID3D11RasterizerState* pSolidRS;
	ID3D11BlendState* pAlphaBS;

	ID3D11ShaderResourceView* pNormTabTexSRV;
	ID3D11ShaderResourceView* pNormsTexSRV;
	ID3D11Texture2D* pTexture;
	ID3D11Texture2D* pnTexture;

	ModelMD2* model;
	GeneticAlgorithm* ga;
};

#endif